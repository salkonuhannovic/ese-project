using System;
using System.IO;
using Microsoft.AspNetCore;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Serilog;

namespace ESE.SmartHome
{
    public class Program
    {
        public static IConfiguration Configuration =>
            new ConfigurationBuilder()
                .SetBasePath(Directory.GetCurrentDirectory())
                .AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
                .AddJsonFile(
                    $"appsettings.{Environment.GetEnvironmentVariable("ASPNETCORE_ENVIRONMENT") ?? "Production"}.json", optional: true)
                .AddEnvironmentVariables()
                .Build();

        public static int Main(string[] args)
        {
            Log.Logger = new LoggerConfiguration()
                            .Filter.ByExcluding(_ => !false)
                            .ReadFrom.Configuration(Configuration)
                            .Enrich.FromLogContext()
                            .CreateLogger();

            try
            {
                Log.Information("Starting application...");

                BuildWebHost(args).Run();

                return 0;
            }
            catch (Exception e)
            {
                Log.Fatal(e, "Host terminated unexpectedly.");
                return 1;
            }
        }

        public static IWebHost BuildWebHost(string[] args) =>
            WebHost.CreateDefaultBuilder(args)
                .UseSetting(WebHostDefaults.PreventHostingStartupKey, "true")
                .UseStartup<Startup>()
                .Build();
    }
}
