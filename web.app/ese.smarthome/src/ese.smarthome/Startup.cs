using System.IO;
using ESE.SmartHome.App.Hubs;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Db;
using ESE.SmartHome.Core.Devices;
using ESE.SmartHome.Core.Measurements;
using ESE.SmartHome.Swagger;
using JetBrains.Annotations;
using MediatR;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.PlatformAbstractions;
using OdeToCode.AddFeatureFolders;
using Swashbuckle.AspNetCore.Swagger;

namespace ESE.SmartHome
{
    [UsedImplicitly]
    public class Startup
    {
        private IConfiguration Configuration { get; }

        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddDbContext<SmartHomeDbContext>(
                    options => options.UseSqlServer(Configuration.GetConnectionString("DefaultConnection"),
                    builder => builder.MigrationsAssembly("ese.smarthome.core")));

            services.AddScoped<IDeviceRepository, DeviceRepository>();
            services.AddScoped<IMeasurementRepository, MeasurementRepository>();

            services.AddScoped<ISmartHomeUnitOfWork, SmartHomeUnitOfWork>();
            
            services.AddMvc(options =>
                {
                    options.Conventions.Add(new ApiGetsOnlyConvention());
                })
                    .AddFeatureFolders(new FeatureFolderOptions { FeatureFolderName = "App" });

            services.AddSignalR();

            services.AddCors(o =>
            {
                o.AddPolicy("Everything", p =>
                {
                    p.AllowAnyHeader()
                        .AllowAnyMethod()
                        .AllowAnyOrigin();
                });
            });

            services.AddMediatR();

            services.AddSwaggerGen(c =>
            {
                c.SwaggerDoc("v1", new Info
                {
                    Title = "SmartHome",
                    Version = "v1",
                    Description =
                        @"To be defined."
                });
                var basePath = PlatformServices.Default.Application.ApplicationBasePath;
                var xmlPath = Path.Combine(basePath, "ese.smarthome.xml");
                c.IncludeXmlComments(xmlPath);
            });
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env, ILogger<Startup> logger, SmartHomeDbContext dbContext)
        {
            if (env.IsDevelopment())
            {
                /*todo add automapper validation*/
                app.UseDeveloperExceptionPage();
            }
            else
            {
                app.UseExceptionHandler("/error");
            }

            app.UseStaticFiles();

            dbContext.Database.Migrate();

            app.UseSwagger();
            app.UseSwaggerUI(c => { c.SwaggerEndpoint("/swagger/v1/swagger.json", "SmartHome V1"); });
            
            app.UseFileServer();
            app.UseCors("Everything");

            app.UseSignalR(routes =>
            {
                routes.MapHub<SensorHub>("sensor");
            });

            app.UseMvc();

            logger.LogInformation("Application started.");
        }
    }
}
