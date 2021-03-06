﻿using System;
using ESE.SmartHome.Core.Entities;

namespace ESE.SmartHome.Core.DeviceSettings
{
    public class DeviceSetting : Entity
    {
        public long DeviceId { get; set; }
        public string Host { get; set; }
        public string Port { get; set; }
        public string MacAddress { get; set; } = string.Empty;
        public int Interval { get; set; }
        public DateTime LastModified { get; set; }
    }
}