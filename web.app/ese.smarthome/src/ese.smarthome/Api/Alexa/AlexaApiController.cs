using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ESE.SmartHome.Api.Shared;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Devices;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace ESE.SmartHome.Api.Alexa
{
    [Route("api/alexa")]
    [AllowAnonymous]
    [Produces("application/json")]
    public class AlexaApiController : Controller
    {
        private readonly ISmartHomeUnitOfWork _unitOfWork;

        public AlexaApiController(ISmartHomeUnitOfWork unitOfWork)
        {
            _unitOfWork = unitOfWork;
        }

        /// <summary>
        /// Get all devices
        /// </summary>
        /// <remarks>
        /// Only active devices are returned.
        /// </remarks>
        /// <returns>All active devices</returns>
        [HttpGet("devices")]
        [ProducesResponseType(typeof(ICollection<DeviceDto>), 200)]
        public async Task<IActionResult> GetAllDevices()
        {
            var devices = await _unitOfWork.Devices.GetAllActiveDevices();

            var dtos = devices.Select(d => new DeviceDto { DeviceId = d.Id, Name = d.Name }).ToList();

            return Ok(dtos);
        }

        /// <summary>
        /// Get measurement for device
        /// </summary>
        /// <remarks>
        /// Returns the most recent measurement for the given device
        /// </remarks>
        /// <param name="deviceId"></param>
        /// <returns>Measurement if successful</returns>
        /// <response code="404">No device with given id found.</response>
        [HttpPost("measurements")]
        [ProducesResponseType(typeof(MeasurementDto), 200)]
        public async Task<IActionResult> GetMeasurmentsByDeviceId([FromBody] long deviceId)
        {
            var measurment = await _unitOfWork.Measurements.GetByDeviceId(deviceId);

            if (measurment == null)
            {
                return NotFound();
            }

            var dto = new MeasurementDto
            {
                DeviceId = measurment.DeviceId,
                Timestamp = measurment.Timestamp,
                Temperature = measurment.Temperature,
                Humidity = measurment.Humidity
            };

            return Ok(dto);
        }

    }
}