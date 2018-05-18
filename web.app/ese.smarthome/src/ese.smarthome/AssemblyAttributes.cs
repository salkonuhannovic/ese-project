// This file is only needed for Resharper due to FeatureFolder usage
// Resolves Resharper Errors on finding Views 
// Furthermore adds Razor View Templates in solution explorer's context menu
using JetBrains.Annotations;

[assembly: AspMvcViewLocationFormat(@"~\App\{1}\{0}.cshtml")]
[assembly: AspMvcViewLocationFormat(@"~\App\Shared\{0}.cshtml")]
[assembly: AspMvcViewLocationFormat(@"~\App\{0}.cshtml")]
[assembly: AspMvcPartialViewLocationFormat(@"~\App\Shared\{0}.cshtml")]
[assembly: AspMvcPartialViewLocationFormat(@"~\App\{1}\{0}.cshtml")]