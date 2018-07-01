using System.Collections.Generic;
using System.Threading.Tasks;
using ESE.SmartHome.Core.Data;
using ESE.SmartHome.Core.Devices;
using MediatR;

namespace ESE.SmartHome.App.RealtimeChart
{
    public class ViewRealtimeChart
    {
        public class Query : IRequest<QueryResult>
        { }

        public class QueryResult
        {
            public IReadOnlyCollection<Device> ActiveDevices { get; set; }
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
                var dbResult = await _unitOfWork.Devices.GetAllActiveDevices();

                var queryResult = new QueryResult
                {
                    ActiveDevices = dbResult
                };

                return await Task.FromResult(queryResult);
            }
        }
    }
}