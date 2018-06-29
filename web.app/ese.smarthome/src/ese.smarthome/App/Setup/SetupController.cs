using System.Threading.Tasks;
using MediatR;
using Microsoft.AspNetCore.Mvc;

namespace ESE.SmartHome.App.Setup
{
    [Route("setup")]
    public class SetupController : Controller
    { 
        private readonly IMediator _mediator;

        public SetupController(IMediator mediator)
        {
            _mediator = mediator;
        }

        [HttpGet("")]
        public async Task<IActionResult> ViewDevices()
        {
            var model = await _mediator.Send(new ViewDeviceSetup.Query());

            return View("ViewDevices", model);
        }
    }
}