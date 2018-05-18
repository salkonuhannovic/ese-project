using System.IO;
using ESE.SmartHome.Core.Db;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;
using Microsoft.Extensions.Configuration;

namespace ESE.SmartHome.DesignTime
{
    public class SmartHomeDbContextFactory : IDesignTimeDbContextFactory<SmartHomeDbContext>
    {
        public SmartHomeDbContext CreateDbContext(string[] args)
        {
            var configuration = new ConfigurationBuilder()
                                            .SetBasePath(Directory.GetCurrentDirectory())
                                            .AddJsonFile("appsettings.json").Build();

            var builder = new DbContextOptionsBuilder<SmartHomeDbContext>();

            var connectionString = configuration.GetConnectionString("DefaultConnection");

            builder.UseSqlServer(connectionString);

            return new SmartHomeDbContext(builder.Options);
        }
    }
}