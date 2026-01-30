using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using TcpCommTest;

namespace TcpCommTest
{
    public class VisionClient
    {
        private TcpClient _client;
        private NetworkStream _stream;
        private Thread _recvThread;
        private Thread _sendThread;
        private bool _running;
        public bool _connected = false;

        //连接状态
        enum ConnectionState
        {
            Disconnected,
            Connecting,
            Connected
        }
        volatile ConnectionState _state;

        //临时存储发送信息类
        public class TcpCommInfo
        {
            public List<string> sentImageIDQueue = new List<string>();
            public List<string> sentImagePathQueue = new List<string>();
        }
        public TcpCommInfo tcpCommInfo = new TcpCommInfo();

        //等待响应的消息字典
        //ConcurrentDictionary<TKey, TValue> :用来接收和存储键值对的消息字典
        //TKey：表示字典中每个元素的键的类型
        //TValue：表示字典中每个元素的值的类型
        private ConcurrentDictionary<string, TaskCompletionSource<JObject>> _waiters
            = new ConcurrentDictionary<string, TaskCompletionSource<JObject>>();
        BlockingCollection<string> ReviceMessage = new BlockingCollection<string>(new ConcurrentQueue<string>());
        private readonly BlockingCollection<string> _sendQueue =
            new BlockingCollection<string>(new ConcurrentQueue<string>());
        private readonly BlockingCollection<JObject> _heartbeatAckQueue = new BlockingCollection<JObject>(new ConcurrentQueue<JObject>());
        private readonly BlockingCollection<JObject> _statusQueue = new BlockingCollection<JObject>(new ConcurrentQueue<JObject>());
        private readonly BlockingCollection<JObject> _inferResultQueue = new BlockingCollection<JObject>(new ConcurrentQueue<JObject>());

        //判断当前连接状态
        public bool IsConnected()
        {
            try
            {
                if(_client == null)
                    return false;
                return _client.Connected;
            }
            catch 
            {
                _state = ConnectionState.Disconnected;
                _connected = false;
                return false; 
            }
        }

        //开始连接
        async Task<bool> ConnectAsync(string ip, int port)
        {
            _state = ConnectionState.Connecting;

            _client = new TcpClient();
            try
            {
                await _client.ConnectAsync(IPAddress.Parse(ip), port);
                _stream = _client.GetStream();

                _state = ConnectionState.Connected;
                _connected = true;

                _running = true;
                _recvThread = new Thread(ReceiveLoop) { IsBackground = true };
                _recvThread.Start();
                StartSendLoop();


                return true;
            }
            catch
            {
                _state = ConnectionState.Disconnected;
                _connected = false;
                return false;
            }
        }

        //连接锁，防止多线程同时连接同一个服务器
        private readonly SemaphoreSlim _connectLock = new SemaphoreSlim(1, 2);
        public async Task<bool> Connect(string ip, int port)
        {
            await _connectLock.WaitAsync();
            try
            {
                if (_client == null || !_client.Connected)
                {
                    Close();
                    await ConnectAsync(ip, port);
                }
            }
            finally
            {
                _connectLock.Release();
            }
            return true;
        }


        //心跳发送并等待响应
        //由于心跳消息是周期性发送的，且要等待响应，所以需要用seq来区分每次发送的心跳消息，并进行有时限的等待
        public async Task<JObject> HeartBeatSendAndWaitAsync(JObject msg, int timeoutMs)
        {
            if (_state != ConnectionState.Connected && !_client.Connected)
                throw new InvalidOperationException("Not connected");

            var tcs = new TaskCompletionSource<JObject>();
            //tcs.SetResult(msg);
            _waiters[msg.Value<string>("seq")] =  tcs;
            await SendAsync(msg);
            using (var cts = new CancellationTokenSource(timeoutMs))
            using (cts.Token.Register(() => { tcs.TrySetCanceled(); } ) )
            {
                try
                {
                    return await tcs.Task;
                }
                finally
                {
                    _waiters.TryRemove(msg.Value<string>("seq"), out _);
                }
            }
        }
        //状态发送并等待响应
        public async Task<JObject> StatusSendAndWaitAsync(JObject msg, int timeoutMs)
        {
            if (_state != ConnectionState.Connected && !_client.Connected)
                throw new InvalidOperationException("Not connected");

            var tcs = new TaskCompletionSource<JObject>();
            //tcs.SetResult(msg);
            _waiters["status_resp"] = tcs;
            await SendAsync(msg);
            using (var cts = new CancellationTokenSource(timeoutMs))
            using (cts.Token.Register(() => { tcs.TrySetCanceled(); }))
            {
                try
                {
                    return await tcs.Task;
                }
                finally
                {
                    _waiters.TryRemove(msg.Value<string>("type"), out _);
                }
            }
        }

        //发送消息锁，防止多线程同时发送消息导致消息混乱
        //最大线程同时发送设置
        private readonly SemaphoreSlim _sendLock = new SemaphoreSlim(1, 11);
        //wield避免多线程同时发送消息导致粘包，添加50ms延时避免粘包
        async Task SendAsync(JObject msg)
        {
            await _sendLock.WaitAsync();
            try
            {
                await Task.Delay(50);
                byte[] data = Encoding.UTF8.GetBytes(msg.ToString(Formatting.None));
                await _stream.WriteAsync(data, 0, data.Length);
            }
            finally
            {
                _sendLock.Release();
            }
        }

        //接收消息循环
        private CancellationTokenSource _revCts;
        private void ReceiveLoop()
        {
            _revCts = new CancellationTokenSource();
            Task.Run(async () =>
            {
                byte[] buf = new byte[128];
                var cache = new MemoryStream();
                try
                {

                    while (!_revCts.IsCancellationRequested)
                    {
                        int bytesRead = await _stream.ReadAsync(buf, 0, buf.Length);
                        if (bytesRead == 0)
                        {
                            // 对端正常关闭
                            //正常情况下，ReadAsync返回0表示连接已关闭
                            Close();
                            break;
                        }

                        // 写入缓存
                        cache.Write(buf, 0, bytesRead);

                        // 尝试拆包
                        ProcessReceiveBuffer(cache);
                    }
                }
                catch
                {
                    Close();
                }
            });
            //多个任务处理接收到的消息
            Task.Run(() =>
            {
                foreach (var msg in ReviceMessage.GetConsumingEnumerable())
                {
                    HandleMessage(msg);
                }
            });
            //
            //争对不同消息类型进行不同处理
            Task.Run(() =>
            {
                foreach (var msg in _heartbeatAckQueue.GetConsumingEnumerable())
                {
                    if (_waiters.TryRemove(msg.Value<string>("seq"), out var tcs))
                    {
                        tcs.TrySetResult(msg);
                    }
                }
            });

            Task.Run(() =>
            {
                foreach (var msg in _inferResultQueue.GetConsumingEnumerable())
                {
                    if (tcpCommInfo.sentImageIDQueue.Count <= 0 && tcpCommInfo.sentImagePathQueue.Count <= 0)
                        continue;
                    //Console.WriteLine($"image info:{msg["fileName"]} ncount:{ncount++}");
                    int pos = tcpCommInfo.sentImageIDQueue.FindIndex(msg.Value<string>("requestId").Equals);
                    string strPath = tcpCommInfo.sentImagePathQueue[pos];
                    if(Directory.Exists(strPath))
                        Directory.Delete(strPath);
                    tcpCommInfo.sentImagePathQueue.Remove(strPath);
                    tcpCommInfo.sentImageIDQueue.Remove(msg["requestId"].ToString());
                }
            });

            Task.Run(() =>
            {
                foreach (var msg in _statusQueue.GetConsumingEnumerable())
                {
                    if(_waiters.TryRemove(msg.Value<string>("type"), out var tcs))
                    {
                        tcs.TrySetResult(msg);
                    }
                }
            });


        }

        //发送消息循环
        private CancellationTokenSource _sendCts;
        private void StartSendLoop()
        {
            _sendCts = new CancellationTokenSource();

            Task.Run(async () =>
            {
                try
                {
                    foreach (var msg in _sendQueue.GetConsumingEnumerable(_sendCts.Token))
                    {
                        if (_stream == null || !_client.Connected)
                            break;
                        //tcs.SetResult(msg);
                        JObject jMsg = JObject.Parse(msg);
                        await SendAsync(jMsg);
                    }
                }
                catch (TaskCanceledException)
                {

                }
                catch (OperationCanceledException)
                {
                    // 正常退出
                }
                catch (Exception ex)
                {

                }
            });
        }

        //消息编码
        //将发送的字符串消息转换为字节数组
        private byte[] EncodeMessage(string msg)
        {
            byte[] msgBytes = Encoding.UTF8.GetBytes(msg);
            //byte[] lengthPrefix = BitConverter.GetBytes(msgBytes.Length);
            byte[] data = new byte[msgBytes.Length];
            //Buffer.BlockCopy(lengthPrefix, 0, data, 0, lengthPrefix.Length);
            Buffer.BlockCopy(msgBytes, 0, data, 0, msgBytes.Length);
            return data;
        }

        //发送消息队列
        //在不考虑接收反馈的情况下，直接将消息添加到发送队列中
        public void SendMessage(string msg)
        {
            _sendQueue.Add(msg);
        }

        //处理接收缓冲区
        private void ProcessReceiveBuffer(MemoryStream cache)
        {
            cache.Position = 0;

            while (true)
            {
                if (cache.Length - cache.Position < 4)
                    break;

                // 读取 JSON
                byte[] lenBytes = new byte[128];
                cache.Read(lenBytes, 0, 128);

                string json = Encoding.UTF8.GetString(lenBytes).TrimEnd('\0');

                // 投递给上层
                ReviceMessage.Add(json);
            }

            // 保留未处理的数据
            var remain = new MemoryStream();
            cache.CopyTo(remain);
            cache.SetLength(0);
            remain.Position = 0;
            remain.CopyTo(cache);
        }

        //处理接收到的消息，根据消息类型进行不同处理
        void HandleMessage(string json)
        {
            JObject obj = JObject.Parse(json);
            string type = (string)obj["type"];

            switch (type)
            {
                case "heartbeat_ack":
                    {
                        _heartbeatAckQueue.Add(obj);
                    }
                    break;

                case "infer_done":
                    _inferResultQueue.Add(obj);
                    break;

                case "status_resp":
                    _statusQueue.Add(obj);
                    break;
            }
        }



        //关闭当前连接，同时初始化相关变量
        public void Close()
        {
            _state = ConnectionState.Disconnected;
            _connected = false;
            _running = false;
            _stream?.Close();
            _client?.Close();
            _stream = null;
            _client = null;
            _revCts?.Cancel();
            _sendCts?.Cancel();

            foreach (var w in _waiters.Values)
                w.TrySetException(new Exception("Connection closed"));
            _waiters.Clear();
        }
    }
}
