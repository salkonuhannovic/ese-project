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
            
            while (!cancellationToken.IsCancellationRequested)
            {
                double tVal = 0.0d;
                double hVal = 0.0d;

                await Task.Delay(1500, cancellationToken);

                // Generate the value to Broadcast to Clients:
                tVal += (20 - rnd.NextDouble() * 1.5);
                hVal += (40 - rnd.NextDouble() * 1.7);

                // Create the Measurement with a Timestamp assigned:
                var measurement = new Measurement
                {
                    Timestamp = DateTime.UtcNow,
                    Temperature = tVal,
                    Humidity = hVal,
                    DeviceId = 2
                };

                Console.WriteLine($"Sending request with Temperature [{measurement.Temperature}] and Humidity [{measurement.Humidity}]");
                
                // Finally send the values:
                await hubConnection.InvokeAsync("Broadcast", "Sensor", measurement, cancellationToken);
            }

            await hubConnection.DisposeAsync();
        }


    }
}
