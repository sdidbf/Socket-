using System;
using System.IO;
using System.Net.Sockets;
using System.Text;

namespace TcpCommTest
{
    public static class LengthPrefixed
    {
        public static void Send(NetworkStream stream, string json)
        {
            byte[] body = Encoding.UTF8.GetBytes(json);
            byte[] len = BitConverter.GetBytes(body.Length);

            //stream.Write(len, 0, 4);
            stream.Write(body, 0, body.Length);
        }

        public static string Receive(NetworkStream stream)
        {
            //byte[] lenBuf = ReadExact(stream, 4);
            //int length = BitConverter.ToInt32(lenBuf, 0);

            byte[] body = ReadExact(stream);
            string str = Encoding.UTF8.GetString(body);
            return str;
        }

        private static byte[] ReadExact(NetworkStream stream)
        {
            byte[] buf = new byte[1024];
            int read = 0;
            while (read <= 0)
            {
                int r = stream.Read(buf, read,1024);
                if (r <= 0) throw new IOException("Socket closed");
                read += r;
            }
            return buf;
        }
    }
}
