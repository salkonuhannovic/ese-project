"use strict";
var Navbar = function () {
    var e = $(".navbar")
        , t = $('.nav-link[href^="#"]')
        , n = $(".navbar-collapse")
        , s = $(window);
    function o() {
        e.removeClass("navbar-light").addClass("navbar-dark");
    }
    function l() {
        e.removeClass("navbar-dark").addClass("navbar-light");
    }
    function i() {
        s.scrollTop() > 5 ? o() : l();
    }
    i(),
        s.on({
            scroll: function () {
                i();
            },
            "activate.bs.scrollspy": function () {
                t.filter(".active").focus();
            }
        }),
        n.on({
            "show.bs.collapse": function () {
                o();
            },
            "hidden.bs.collapse": function () {
                0 == s.scrollTop() && l();
            }
        }),
        t.on("click", function () {
            n.collapse("hide");
        });
    }()
    , Menu = function () {
        var e = $(".section_menu__grid")
            , t = $(".section_menu__nav")
            , n = ".section_menu__grid__item";
        e.length && e.each(function () {
            var e = $(this)
                , s = e.attr("id")
                , o = t.find('li.active > a[href="#' + s + '"]').data("filter")
                , l = e.isotope({
                    itemSelector: n,
                    filter: o
                });
            l.imagesLoaded().progress(function () {
                l.isotope("layout");
            });
            }),
            t.on("click", "li > a", function (t) {
                t.preventDefault();
                var n, s, o, l, i = $(this);
                n = i,
                    s = e.filter(n.attr("href")),
                    o = n.data("filter"),
                    s.isotope({
                        filter: o
                    }),
                    (l = i).parent("li").siblings("li").removeClass("active"),
                    l.parent("li").addClass("active");
            });
    }()
    , Events = function () {
        var e = $(".section_events__items")
            , t = $(".section_events__item")
            , n = $(".section_events__item__content_sm");
        $(".section_events__item__content_lg");
        n.on("click", function () {
            var n;
            (n = $(this)).closest(e).find(t).removeClass("active"),
                n.closest(t).addClass("active");
        });
    }()
    , CurrentDate = function () {
        var e, t = $("#js-current-year");
        t.length && (e = (new Date).getFullYear(),
            t.text(e));
    }();
