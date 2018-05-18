/// <binding BeforeBuild='copy' />
var gulp    = require("gulp"),
    flatten = require("gulp-flatten"),
    fs      = require("fs"),
    del     = require("del"),
    rs      = require("run-sequence");

var cssDir   = "wwwroot/css/vendor",
    jsDir    = "wwwroot/js/vendor",
    fontsDir = "wwwroot/css/fonts";

gulp.task("copy", ["copy:js", "copy:css", "copy:fonts"]);

gulp.task("copy:js", function () {
    var asset = getFiles("js");
    return gulp.src(asset.files, { base: "." })
        .pipe(flatten())
        .pipe(gulp.dest(jsDir));
});

gulp.task("copy:css", function () {
    var asset = getFiles("css");
    return gulp.src(asset.files, { base: "." })
        .pipe(flatten())
        .pipe(gulp.dest(cssDir));
});

gulp.task("copy:fonts", function() {
    var asset = getFiles("fonts");
    return gulp.src(asset.files, { base: "." })
        .pipe(flatten())
        .pipe(gulp.dest(fontsDir));
});

gulp.task("clean", function () {
    return del(new Array(cssDir, jsDir, fontsDir));
});

gulp.task("watch", function () {
    gulp.watch("./assetsconfig.json", () => rs("clean", "copy"));
});

function getFiles(target) {
    var assetsconfig = JSON.parse(fs.readFileSync("./assetsconfig.json"));
    return assetsconfig.filter(function (assets) {
        return assets.target === target;
    })[0];
}