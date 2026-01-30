using Newtonsoft.Json;
using System;

namespace TcpCommTest
{
    public class VisionMessage
    {
        public string version { get; set; } = "1.0";
        public string type { get; set; }
        public int seq { get; set; }
        public long timestamp { get; set; }
        public object payload { get; set; }
    }

    public static class VisionMessageFactory
    {
        private static int _seq = 0;

        public static VisionMessage Create(string type, object payload)
        {
            return new VisionMessage
            {

                type = type,
                seq = System.Threading.Interlocked.Increment(ref _seq),
                timestamp = DateTimeOffset.UtcNow.Millisecond,
                payload = payload
            };
        }
    }
}
