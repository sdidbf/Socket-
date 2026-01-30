using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TcpCommTest
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new TcpComm());
        }
        //static async Task Main()
        //{
        //    var client = new VisionClient();
        //    client.Connect("192.168.0.197", 9000);

        //    // 心跳
        //    var hb = VisionMessageFactory.Create(
        //        "heartbeat_req",
        //        new { machineId = "VISION_PC_01" }
        //    );

        //    var hbResp = await client.SendAndWaitAsync(hb);
        //    Console.WriteLine("Heartbeat Resp: " + hbResp.type);

        //    // 推理请求
        //    var infer = VisionMessageFactory.Create(
        //        "infer_req",
        //        new
        //        {
        //            taskId = "TASK001",
        //            imagePath = @"D:\img.jpg",
        //            model = "yolo_v8"
        //        }
        //    );

        //    var inferResp = await client.SendAndWaitAsync(infer, 10000);
        //    Console.WriteLine("Infer Resp: " + inferResp.payload);

        //    Console.ReadLine();
        //}
    }
}
