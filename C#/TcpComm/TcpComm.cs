using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TcpCommTest
{
    public partial class TcpComm : Form
    {
        public TcpComm()
        {
            InitializeComponent();
            LoadData();
            _statusCts = new CancellationTokenSource();
            _ = SetStatusAsync(_statusCts.Token);

            Run();
        }
        private readonly string Server_IP = ConfigurationManager.AppSettings["Server-IP"].ToString();
        private readonly string Server_Port = ConfigurationManager.AppSettings["Server-Port"].ToString();
        private readonly string Client_IP = ConfigurationManager.AppSettings["Client-IP"].ToString();
        private readonly string Client_Port = ConfigurationManager.AppSettings["Client-Port"].ToString();
        private readonly string DetectModual = ConfigurationManager.AppSettings["DetectModual"].ToString();
        private readonly string EqpName = ConfigurationManager.AppSettings["EqpName"].ToString();
        private readonly string LogPath = ConfigurationManager.AppSettings["LogPath"].ToString();
        private readonly string ImagePath = ConfigurationManager.AppSettings["ImagePath"].ToString();
        private int seq = 0;
        private int nHeartBeat = 0;
        private int nStatusBeat = 0;
        private bool bMinorAlarm = false;
        private DateTime oldTime = new DateTime(0); //记录上次读取文件夹图片最新的时间
        private DateTime newTime = new DateTime(0); //记录本次读取文件夹中图片的最新时间
        private Queue<string> sentImagePathQueue = new Queue<string>();
        private Queue<string> sentImageNameQueue = new Queue<string>();
        private List<string> sentImageIDQueue = new List<string>();
        private bool bReadingImages = true;
        readonly VisionClient client = new VisionClient();
        readonly VisionClient server = new VisionClient();

        //简单加载页面配置
        private void LoadData()
        {
            tbxIpAddressServer.Text = Server_IP;
            tbxIpPortServer.Text = Server_Port;
            tbxIpAddressClient.Text = Client_IP;
            tbxIpPortClient.Text = Client_Port;
            cbxDetectModual.Text = DetectModual;
            
        }
        private CancellationTokenSource _statusCts;

        //设置通信状态显示
        private async Task SetStatusAsync(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                btnConnectStatusServer.BackColor =
                    client._connected ? Color.Lime : Color.Red;

                btnConnectStatusClient.BackColor =
                    server._connected ? Color.Lime : Color.Red;
                await Task.Delay(300, token); // 300ms 刷新一次
            }
        }

        //读取文件夹中的图片
        private bool ReadImagesInFolder(string folderPath)
        {
            if (!Directory.Exists(folderPath))
            {
                Directory.CreateDirectory(folderPath);
                return false;
            }

            string[] extensions = { ".jpg", ".png", ".bmp", ".jpeg" };

            var files = Directory
                .GetFiles(folderPath)
                .Where(f => extensions.Contains(Path.GetExtension(f).ToLower()));

            foreach (var file in files)
            {
                FileInfo fi = new FileInfo(file);

                if (oldTime >= fi.CreationTime)
                    return false;

                if(newTime< fi.CreationTime)
                    newTime = fi.CreationTime;
                //文件名: fi.Name
                //创  建: fi.CreationTime       :yyyy-MM-dd HH:mm:ss
                //修  改: fi.LastWriteTime      :yyyy-MM-dd HH:mm:ss
                sentImagePathQueue.Enqueue(fi.FullName);
                sentImageNameQueue.Enqueue(fi.Name);
                sentImageIDQueue.Add(fi.Name.Replace(".png","_") + $"{fi.CreationTime:yyMMddHHmmss}");
                //Console.WriteLine(
                //    $"文件名: {fi.Name}, " +
                //    $"创建: {fi.CreationTime:yyyy-MM-dd HH:mm:ss}, " +
                //    $"修改: {fi.LastWriteTime:yyyy-MM-dd HH:mm:ss}"
                //);
            }
            if(newTime > oldTime)
                oldTime = newTime;
            return true;
        }

        //工作函数
        private async void Run()
        {
            await client.Connect(Server_IP, int.Parse(Server_Port));


            //Heartbeat请求
            CancellationTokenSource hbCts = new CancellationTokenSource();
            Task heartbeatTask = Task.Run(async () =>
            {
                while (!hbCts.Token.IsCancellationRequested)
                {
                    try
                    {
                        if (client.IsConnected())
                        {
                            JObject heartbeat = new JObject
                            {
                                ["type"] = "heartbeat_req",
                                ["machineId"] = EqpName,
                                ["seq"] = seq
                            };
                            // 发送后等待3s，三秒内没有接收到响应则视为心跳失败
                            var revData = await client.HeartBeatSendAndWaitAsync(heartbeat, 3000);
                            seq++;
                            nHeartBeat = 0;
                        }
                        else
                        {
                            await client.Connect(Server_IP, int.Parse(Server_Port));
                        }
                    }
                    catch (TaskCanceledException ex1) 
                    {
                        nHeartBeat++;
                        if(nHeartBeat>20)
                        {
                            // 超过20次心跳取消，触发重报警
                            nHeartBeat = 0;
                            bMinorAlarm = false;
                            LogAlarmAsync("Heart Beat MajorAlarm");
                            client.Close();
                        }
                        else if (nHeartBeat > 3 && !bMinorAlarm)
                        {
                            bMinorAlarm = true;
                            // 超过三次心跳取消，触发轻报警
                            LogAlarmAsync("Heart Beat MinorAlarm");
                            client.Close();

                        }
                        Log($"\nHeartbeatTask TaskCanceledException(nHeartBeat:{nHeartBeat}):\n" + ex1.ToString());
                    }
                    catch (Exception ex)
                    {

                        Log("\nHeartbeatTask Exception(nHeartBeat:{nHeartBeat}):\n" + ex.ToString());
                        //Log.Error(ex);
                    }
                    await Task.Delay(1000, hbCts.Token);
                }
                int a = 0;
            });

            CancellationTokenSource imgCts = new CancellationTokenSource();
            //图片发送请求
            Task ImageSendTask = Task.Run(async () =>
            {
                while (!imgCts.Token.IsCancellationRequested)
                {
                    try
                    {
                        // 读取图片文件夹中的图片并发送
                        if (bReadingImages && !ReadImagesInFolder(ImagePath))
                        {
                            await Task.Delay(3000, imgCts.Token);
                            continue;
                        }
                        bReadingImages = false;
                        if (client.IsConnected())
                        {
                            JObject heartbeat = new JObject
                            {
                                ["type"] = "status_query",
                                ["machineId"] = EqpName,
                                //["seq"] = seq
                            };
                            // 只发送，不等待
                            var Result = await client.StatusSendAndWaitAsync(heartbeat,3000);
                            int nPending = Result.Value<int>("pending");
                            int nLimit = Result.Value<int>("limit");
                            client.tcpCommInfo.sentImageIDQueue.AddRange(sentImageIDQueue);
                            client.tcpCommInfo.sentImagePathQueue.AddRange(sentImagePathQueue.ToList());
                            for (int i = 0; i < nLimit - nPending; i++)
                            {
                                if(sentImageNameQueue.Count<=0)
                                    break;
                                JObject imgMsg = new JObject
                                {
                                    ["type"]        = "infer_request",
                                    ["machineId"]   = EqpName,
                                    ["requestId"]   = sentImageIDQueue[i],      //设置的图片唯一标识ID
                                    ["uncPath"]     = sentImagePathQueue.ElementAt(i),   //路径
                                    ["fileName"]    = sentImageNameQueue.ElementAt(i),
                                    ["modelKey"]    = DetectModual
                                };
                                client.SendMessage(imgMsg.ToString());
                            }
                            bReadingImages = true;
                            nStatusBeat = 0;
                        }
                        else
                        {
                            await client.Connect(Server_IP, int.Parse(Server_Port));
                        }
                        await Task.Delay(1000, imgCts.Token);
                    }
                    catch (TaskCanceledException ex1)
                    {
                        nStatusBeat++;
                        if (nStatusBeat > 20)
                        {
                            // 超过20次状态获取失败，触发重报警
                            nStatusBeat = 0;
                            bMinorAlarm = false;
                            LogAlarmAsync("Status MajorAlarm");
                        }
                        else if (nStatusBeat > 3 && !bMinorAlarm)
                        {
                            bMinorAlarm = true;
                            // 超过三次状态获取失败，触发轻报警
                            LogAlarmAsync("Status MinorAlarm");

                        }
                        Log("\nTaskCanceledException:\n" + ex1.ToString());
                    }
                    catch (Exception ex)
                    {
                        Log("\nImageSendTask Exception:\n" + ex.ToString());
                        //Log.Error(ex);
                    }
                    //await Task.Delay(3000, hbCts.Token);
                }
                int b = 0;
            });
        }

        //日志记录
        private readonly SemaphoreSlim _LogLock = new SemaphoreSlim(1, 1);
        private async void Log(string msg)
        {
            await _LogLock.WaitAsync();
            try
            {
                DateTime dt = DateTime.Now;
                string path = LogPath + "/Exception/" + dt.Month.ToString();
                if (!Directory.Exists(path)) Directory.CreateDirectory(path);
                File.AppendAllText($"{path}/{dt:yyyyMMdd}.log", $"\n{dt} {msg}\r\n");
            }
            finally
            {
                _LogLock.Release();
            }
        }
        private readonly SemaphoreSlim _LogAlarmLock = new SemaphoreSlim(1, 1);
        private async void LogAlarmAsync(string msg)
        {
            await _LogLock.WaitAsync();
            try
            {
                DateTime dt = DateTime.Now;
                string path = LogPath + "/Alarm/" + dt.Month.ToString();
                if (!Directory.Exists(path)) Directory.CreateDirectory(path);
                File.AppendAllText($"{path}/{dt:yyyyMMdd}.log", $"nHeartBeat:{nHeartBeat}\n{dt} {msg}\r\n");
            }
            finally
            {
                _LogLock.Release();
            }
        }
    }
}
