using System.Threading.Tasks;
using ESE.SmartHome.Core.Measurements;
using JetBrains.Annotations;
using Microsoft.AspNetCore.SignalR;

namespace ESE.SmartHome.App.Hubs
{
    [UsedImplicitly]
    public class SensorHub : Hub
    {
        public Task Broadcast(string sender, Measurement measurement)
        {
            return Clients
                // Do not Broadcast to Caller:
                .AllExcept(new[]
                {
                    Context.ConnectionId
                })
                // Broadcast to all connected clients:
                .InvokeAsync("Broadcast", sender, measurement);
        }
    }
}