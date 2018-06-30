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

        [HttpGet("devices")]
        public async Task<IActionResult> ViewDevices()
        {
            var model = await _mediator.Send(new ViewDeviceSetup.Query());

            return View("ViewDevices", model);
        }

        [HttpGet("add")]
        public IActionResult AddDevice()
        {
            return PartialView("AddDevice", new AddDevice.Command());
        }

        [HttpPost("add")]
        public async Task<IActionResult> AddDevice(AddDevice.Command command)
        {
            if (!ModelState.IsValid)
            {
                return View(command);
            }

            await _mediator.Send(command);

            return RedirectToAction("ViewDevices");
        }

        [HttpGet("edit")]
        public async Task<IActionResult> EditDevice([FromQuery] EditDevice.Query query)
        {
            var command = await _mediator.Send(query);

            return PartialView("EditDevice", command);
        }

        [HttpPost("edit")]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> EditDevice(EditDevice.Command command)
        {
            await _mediator.Send(command);
            TempData["TempMessage"] = "Device erfolgreich aktualisiert.";

            return RedirectToAction("ViewDevices");
        }

        [HttpGet("delete")]
        public async Task<IActionResult> DeleteDevice(DeleteDevice.Query query)
        {
            var command = await _mediator.Send(query);

            return PartialView("DeleteDevice", command);
        }

        [HttpPost("delete")]
        public async Task<IActionResult> Delete(DeleteDevice.Command command)
        {
            await _mediator.Send(command);

            TempData["Msg"] = $"Sensor {command.Name} gelöscht.";
            
            return RedirectToAction("ViewDevices");
        }

    }
}