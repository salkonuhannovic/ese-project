using Microsoft.AspNetCore.Mvc.ApplicationModels;

namespace ESE.SmartHome.Swagger
{
    public class ApiGetsOnlyConvention : IActionModelConvention
    {
        public void Apply(ActionModel action)
        {
            action.ApiExplorer.IsVisible = action.Controller.ControllerName.Contains("Api");
        }
    }
}