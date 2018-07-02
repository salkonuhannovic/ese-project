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
            var model = await _mediator.Send(new ViewDashboard.Query());
         
            return View("ViewDashboard", model);
        }
    }
}