using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ESE.SmartHome.Api.Shared;
using ESE.SmartHome.Core.Data;
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
        /// Get all active devices
        /// </summary>
        /// <remarks>
        /// Only active devices are returned.
        /// </remarks>
        /// <returns>All active devices</returns>
        [HttpGet("devices")]
        [ProducesResponseType(typeof(ICollection<DeviceDto>), 200)]
        public async Task<IActionResult> GetActiveDevices()
        {
            var devices = await _unitOfWork.Devices.GetAllActiveDevices();

            if (!devices.Any())
            {
                return NotFound();
            }

            var dtos = devices.Select(d => new DeviceDto { DeviceId = d.Id, Name = d.Name }).ToList();

            return Ok(dtos);
        }

        /// <summary>
        /// Get measurement for device
        /// </summary>
        /// <remarks>
        /// Returns the most recent measurement for the given device
        /// </remarks>
        /// <param name="deviceName"></param>
        /// <returns>Measurement if successful</returns>
        /// <response code="204">No measurments for device with given id found.</response>
        [HttpGet("measurements/{deviceName}")]
        [ProducesResponseType(typeof(MeasurementDto), 200)]
        public async Task<IActionResult> GetMeasurmentsByDeviceId(string deviceName)
        {
            var devices = await _unitOfWork.Devices.GetAllActiveDevices();
            
            var deviceFound = devices.FirstOrDefault(d => d.Name.ToLowerInvariant().Equals(deviceName.ToLowerInvariant()));
            if (deviceFound == null)
            {
                return NoContent();
            }

            var measurment = await _unitOfWork.Measurements.GetByDeviceId(deviceFound.Id);
            if (measurment == null)
            {
                return NoContent();
            }

            var dto = new MeasurementDto
            {
                DeviceId = measurment.DeviceId,
                Timestamp = measurment.Timestamp,
                Temperature = Math.Truncate(measurment.Temperature * 100) / 100,
                Humidity = Math.Round(measurment.Humidity)
            };

            return Ok(dto);
        }

    }
}