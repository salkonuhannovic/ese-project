using System.Threading.Tasks;
using ESE.SmartHome.Core.Measurements;
using Microsoft.AspNetCore.SignalR;

namespace ESE.SmartHome.App.Hubs
{
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