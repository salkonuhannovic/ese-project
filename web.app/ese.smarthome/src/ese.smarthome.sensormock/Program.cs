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

            double value = 0.0d;

            while (!cancellationToken.IsCancellationRequested)
            {
                await Task.Delay(1000, cancellationToken);

                // Generate the value to Broadcast to Clients:
                value = Math.Min(Math.Max(value + (0.1 - rnd.NextDouble() / 5.0), -1), 1);

                // Create the Measurement with a Timestamp assigned:
                var measurement = new Measurement
                {
                    Timestamp = DateTime.UtcNow,
                    Temperature = value
                };
                
                // Finally send the value:
                await hubConnection.InvokeAsync("Broadcast", "Sensor", measurement, cancellationToken);
            }

            await hubConnection.DisposeAsync();
        }


    }
}
