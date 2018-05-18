using System;
using System.Threading.Tasks;
using MediatR;
using Microsoft.AspNetCore.Mvc;

namespace ESE.SmartHome.App.Dashboard
{
    [Route("dashboard")]
    public class DashboardController : Controller
    {
        private readonly IMediator _mediator;

        public DashboardController(IMediator mediator)
        {
            _mediator = mediator;
        }

        [HttpGet("")]
        public async Task<IActionResult> ViewDashboard()
        {
            var ts = new TimeSpan(7, 0, 0);

            var query = new ViewDashboard.Query { TimeSpan = ts };

            var result = await _mediator.Send(query);

            return View("ViewDashboard", result);
        }
    }
}