using System;
using System.Threading.Tasks;
using ESE.SmartHome.App.Hubs;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Measurements;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.SignalR;

namespace ESE.SmartHome.Api.Sensors
{
    [Route("api/measurements")]
    public class SensorApiController : Controller
    {
        private readonly IHubContext<SensorHub> _hubContext;
        private readonly ISmartHomeUnitOfWork _unitOfWork;

        public SensorApiController(IHubContext<SensorHub> hubContext, ISmartHomeUnitOfWork unitOfWork)
        {
            _hubContext = hubContext;
            _unitOfWork = unitOfWork;
        }

        /// <summary>
        /// Add measurement data
        /// </summary>
        /// <param name="dto"></param>
        /// <returns></returns>
        [HttpPost]
        [Consumes("application/json")]
        [ProducesResponseType(typeof(void), 204)]
        public async Task<IActionResult> AddMeasurement([FromBody] MeasurementDto dto)
        {
            var measurement = new Measurement
            {
                DeviceId = dto.DeviceId,
                Temperature = dto.Temperature,
                Humidity = dto.Humidity,
                Timestamp = DateTime.UtcNow
            };

            _unitOfWork.Measurements.Add(measurement);
            _unitOfWork.Save();

            //var measurement = new Measurement {Timestamp = dto.Timestamp, Temperature = dto.Temperature};

            await _hubContext.Clients.All.InvokeAsync("Broadcast", "Sensor", measurement);

            return NoContent();
        }

    }
}