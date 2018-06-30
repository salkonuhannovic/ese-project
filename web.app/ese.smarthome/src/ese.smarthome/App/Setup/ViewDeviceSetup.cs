using System.Collections.Generic;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Devices;
using MediatR;

namespace ESE.SmartHome.App.Setup
{
    public class ViewDeviceSetup
    {
        public class Query : IRequest<QueryResult>
        { }

        public class QueryResult
        {
            public IReadOnlyCollection<Device> Devices { get; set; }
        }

        public class QueryHandler : AsyncRequestHandler<Query, QueryResult>
        {
            private readonly ISmartHomeUnitOfWork _unitOfWork;

            public QueryHandler(ISmartHomeUnitOfWork unitOfWork)
            {
                _unitOfWork = unitOfWork;
            }

            protected override async Task<QueryResult> HandleCore(Query request)
            {
                var dbResult = await _unitOfWork.Devices.GetAllDevices();

                var queryResult = new QueryResult
                {
                    Devices = dbResult
                };

                return await Task.FromResult(queryResult);
            }
        }
    }
}