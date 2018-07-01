using Microsoft.AspNetCore.Mvc;

namespace ESE.SmartHome.Api.Swagger
{
    [ApiExplorerSettings(IgnoreApi = true)]
    public class SwaggerRouteController : Controller
    {
        [Route("/")]
        [Route("/docs")]
        [Route("/swagger")]
        public IActionResult Index()
        {
            return new RedirectResult("~/swagger");
        }
    }
}