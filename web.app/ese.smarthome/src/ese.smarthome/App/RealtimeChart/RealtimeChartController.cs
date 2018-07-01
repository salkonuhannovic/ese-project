using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using ESE.SmartHome.App.RealtimeChart;
using ESE.SmartHome.Core.Devices;
using MediatR;
using Microsoft.AspNetCore.Mvc;

namespace ese.smarthome.App.RealtimeChart
{
    [Route("realtime-chart")]
    public class RealtimeChartController : Controller
    {
        private readonly IMediator _mediator;

        public RealtimeChartController(IMediator mediator)
        {
            _mediator = mediator;
        }

        [HttpGet("")]
        public async Task<IActionResult> ViewRealtimeChart()
        {
            var result = await _mediator.Send(new ViewRealtimeChart.Query());

            if (!result.ActiveDevices.Any())
            {
                return NoContent();
            }

            return View("ViewRealtimeChart", result);
        }
    }
}