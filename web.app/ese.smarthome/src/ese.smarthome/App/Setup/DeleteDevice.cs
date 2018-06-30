using System.ComponentModel.DataAnnotations;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Data;
using MediatR;

namespace ESE.SmartHome.App.Setup
{
    public class DeleteDevice
    {
        public class Query : IRequest<Command>
        {
            public long DeviceId { get; set; }
        }

        public class Command : IRequest
        {
            public long DeviceId { get; set; }
            public string Name { get; set; }
        }

        public class QueryHandler : AsyncRequestHandler<Query, Command>
        {
            private readonly ISmartHomeUnitOfWork _unitOfWork;

            public QueryHandler(ISmartHomeUnitOfWork unitOfWork)
            {
                _unitOfWork = unitOfWork;
            }

            protected override async Task<Command> HandleCore(Query request)
            {
                var device = await _unitOfWork.Devices.GetByIdAsync(request.DeviceId);

                var command = new Command
                {
                    DeviceId = device.Id,
                    Name = device.Name
                };

                return command;
            }
        }

        public class CommandHandler : AsyncRequestHandler<Command>
        {
            private readonly ISmartHomeUnitOfWork _unitOfWork;

            public CommandHandler(ISmartHomeUnitOfWork unitOfWork)
            {
                _unitOfWork = unitOfWork;
            }

            protected override async Task HandleCore(Command request)
            {
                var device = await _unitOfWork.Devices.GetByIdAsync(request.DeviceId);

                _unitOfWork.Devices.Delete(device);

                await _unitOfWork.SaveAsync();
            }
        }
    }
}