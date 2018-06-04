﻿using System;
using System.Collections.Generic;
using ESE.SmartHome.Core.Entities;
using ESE.SmartHome.Core.Measurements;

namespace ESE.SmartHome.Core.Devices
{
    public class Device : Entity
    {
        public string Name { get; set; }
        public DateTime Created { get; set; }
        public bool IsActive { get; set; }

        public IList<Measurement> Measurements { get; set; }
    }
}