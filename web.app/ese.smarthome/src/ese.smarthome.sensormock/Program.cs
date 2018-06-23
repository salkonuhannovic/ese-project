using System;
using System.Threading;
using System.Threading.Tasks;
using ESE.SmartHome.SensorMock.Mocks;
using Microsoft.AspNetCore.SignalR.Client;

namespace ese.smarthome.sensormock
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var cancellationTokenSource = new CancellationTokenSource();

            Task.Run(() => MainAsync(cancellationTokenSource.Token).GetAwaiter().GetResult(), cancellationTokenSource.Token);

            Console.WriteLine("Press Enter to Exit ...");
            Console.ReadLine();

            cancellationTokenSource.Cancel();
        }

        private static async Task MainAsync(CancellationToken cancellationToken)
        {
            var hubConnection = new HubConnectionBuilder()
                .WithUrl("http://localhost:57237/sensor")
                .Build();

            await hubConnection.StartAsync();

            // Initialize a new Random Number Generator:
            var rnd = new Random();

            double tVal = 0.0d;
            double hVal = 0.0d;

            while (!cancellationToken.IsCancellationRequested)
            {
                await Task.Delay(1000, cancellationToken);

                // Generate the value to Broadcast to Clients:
                tVal = Math.Min(Math.Max(tVal + (0.1 - rnd.NextDouble() * 10), -1), 1);
                
                hVal = Math.Min(Math.Max(hVal + (0.1 - rnd.NextDouble() * 13), -1), 1);

                // Create the Measurement with a Timestamp assigned:
                var measurement = new Measurement
                {
                    Timestamp = DateTime.UtcNow,
                    Temperature = tVal,
                    Humidity = hVal,
                    DeviceId = 1
                };
                
                // Finally send the value:
                await hubConnection.InvokeAsync("Broadcast", "Sensor", measurement, cancellationToken);
            }

            await hubConnection.DisposeAsync();
        }


    }
}
