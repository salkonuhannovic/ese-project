using Microsoft.AspNetCore.Diagnostics;
using Microsoft.AspNetCore.Mvc;

namespace ESE.SmartHome.App.Errors
{
    public class ErrorsController : Controller
    {
        [Route("error")]
        public IActionResult Error()
        {
            var exceptionFeature = HttpContext.Features
                .Get<IExceptionHandlerPathFeature>();

            if (exceptionFeature != null)
            {
                var exception = exceptionFeature.Error;
                var path = exceptionFeature.Path;

                var model = new Error
                {
                    ErrorMessage = exception.Message,
                    RequestedPath = path
                };

                return View(model);
            }

            return View();
        }
    }

    public class Error
    {
        public string RequestedPath { get; set; }
        public string ErrorMessage { get; set; }
    }

}