using System.Threading.Tasks;
using ESE.SmartHome.App.RealtimeChart;
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
            
            return View("ViewRealtimeChart", result);
        }
    }
}