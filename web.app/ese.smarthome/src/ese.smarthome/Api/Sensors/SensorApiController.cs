using System;
using System.Linq;
using System.Threading.Tasks;
using ESE.SmartHome.Api.Shared;
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

        [HttpGet("/setup/{mac}")]
        public async Task<IActionResult> GetDeviceId(string mac)
        {
            if (string.IsNullOrWhiteSpace(mac))
            {
                return BadRequest();
            }

            var devices = await _unitOfWork.Devices.GetAllActiveDevices();
            var deviceId =
                devices.Where(d => d.Setting.MacAddress.ToLowerInvariant().Equals(mac.ToLowerInvariant())).ToList();
            
            if (!deviceId.Any())
            {
                return NotFound();
            }

            var id = deviceId.Select(d => d.Id).FirstOrDefault();

            return Ok(id);
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
                Timestamp = DateTime.Today
            };

            var m = await _unitOfWork.Measurements.GetByDeviceId(dto.DeviceId);
            if (m == null)
            {
                _unitOfWork.Measurements.Add(measurement);
                _unitOfWork.Save();
            }
            else
            {
                m.Temperature = dto.Temperature;
                m.Humidity = dto.Humidity;
                m.Timestamp = DateTime.Today;

                await _unitOfWork.Measurements.UpdateAsync(m);
            }

            await _hubContext.Clients.All.InvokeAsync("Broadcast", "Sensor", measurement);

            return NoContent();
        }

    }
}