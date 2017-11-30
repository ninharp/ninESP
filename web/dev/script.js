<!--[if lt IE 9]>
<SCRIPT src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></SCRIPT>
<SCRIPT src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></SCRIPT>     
<![endif]-->

$(document).ready(function() {
    App.GetNetworks(!1), App.Events.ReloadNetworks();
});

var App = {
    GetNetworksURL: "/ajax/get-networks",
    ConnectURL: "/ajax/connect",
    Connect: null,
    CurrentNetwork: null,
    GetNetworks: function(t) {
        !function e() {
            $("#floatingCirclesG").show();
            var n = $.ajax({
                url: App.GetNetworksURL,
                type: "POST",
                data: {
                    reconnect: t
                },
                success: function(t) {
                    App.SetNetworks(t.available, t.network), App.ShowConnectToNetwork(), App.ConnectToNetwork(), 
                    t.network && App.Utilities.ShowCurrentNetwork($('div[data-name="' + t.network + '"]').parent());
                }
            }).done(function() {
                $("#floatingCirclesG").hide();
            }).fail(function() {
                setTimeout(e, 5e3);
            });
            App.Utilities.AbortConnection(n);
        }();
    },
    ConnectToNetwork: function() {
        $(".connect-btn").unbind("click"), $(".connect-btn").on("click", function() {
            var t = this, e = $(t).parent().parent().parent(), n = e.find("input[type=password]").val(), i = e.find("h4").text();
            App.Utilities.ShowLoader(t), App.Connect && (clearInterval(App.Connect), App.Utilities.HideLoader()), 
            App.Utilities.ShowLoader(t), function o() {
                $(".error").empty();
                var r = $.ajax({
                    url: App.ConnectURL,
                    type: "POST",
                    data: {
                        network: i,
                        password: n
                    },
                    success: function(r) {
                        1 == r.connected && ($(t).parent().parent().empty(), App.Utilities.HideLoader(), 
                        App.Utilities.ShowCurrentNetwork(e)), r.error ? ($(".error").append('<div class="alert alert-danger" role="alert"><span class="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span><span>Error:</span> ' + r.error + "</div>"), 
                        App.Utilities.HideLoader()) : 0 == r.connected && (1 == r.status ? (i = null, n = null, 
                        App.Connect = setTimeout(o, 50)) : App.Utilities.HideLoader());
                    },
                    error: function() {
                        setTimeout(o, 5e3);
                    }
                });
                App.Utilities.AbortConnection(r);
            }();
        });
    },
    SetNetworks: function(t, e) {
        var n = App.Utilities.GetNetworksId();
        $.each(t, function(t, i) {
            n.indexOf(i.id) != -1 ? (App.Utilities.UpdateNetworkValues($(".network[data-id=" + i.id + "]"), i), 
            n.splice(n.indexOf(i.id), 1)) : App.AddNetwork(i, e);
        }), $.each(n, function(t, e) {
            App.Utilities.RemoveNetwork($(".network[data-id=" + e + "]"));
        });
    },
    ShowConnectToNetwork: function() {
        $(".network").unbind("click"), $(".network").on("click", function(t) {
            $(".connect").each(function() {
                $(this).hide();
            }), $(this).find(".connect").show();
        });
    },
    AddNetwork: function(t, e) {
        var n = "";
        if (t.title != e) {
            var i = "";
            "OPEN" != t.encryption && (i += '<label>Password: </label><input class="form-control" style="width: 200px" type="password">'), 
            n += i + '<div class="connect-btn-wrapper"><button class="btn btn-success connect-btn">Connect</button></div>';
        }
        return $(".networks").append('<div class="list-group-item network" href="#" data-id="' + t.id + '"><p class="list-group-item-text"><div><div class="wifi ' + App.Utilities.GetNetworkSignalClass(t.signal) + '"></div><div class="network-info"><h4 class="list-group-item-heading">' + t.title + '</h4><div><span class="encryption"> ' + t.encryption + '</span></div></div></div></p><div class="form-group connect"  style="display: none" data-name="' + t.title + '">' + n + "</div></div>"), 
        !0;
    },
    Events: {
        ReloadNetworks: function() {
            $(".reload").on("click", function() {
                App.GetNetworks(!0);
            });
        }
    },
    Utilities: {
        AbortConnection: function(t) {
            4 != t.readyState && setTimeout(function() {
                t.abort();
            }, 6e3);
        },
        RemoveNetwork: function(t) {
            $(t).remove();
        },
        GetNetworkSignalClass: function(t) {
            return t >= -100 && t <= -80 ? "wifi-1" : t > -80 && t <= -65 ? "wifi-2" : t > -65 && t < -50 ? "wifi-3" : "wifi-4";
        },
        UpdateNetworkValues: function(t, e) {
            $(t).find("h4").text(" " + e.title), $(t).find(".encryption").text(" " + e.encryption), 
            $(t).find(".wifi").removeClass().addClass("wifi").addClass(App.Utilities.GetNetworkSignalClass(e.signal));
        },
        ShowCurrentNetwork: function(t) {
            console.log(t), App.CurrentNetwork && $(App.CurrentNetwork).css("background", ""), 
            App.CurrentNetwork = t, $(App.CurrentNetwork).css("background", "#5BC0DE");
        },
        GetNetworksId: function() {
            var t = [];
            return $(".network").each(function() {
                t.push(parseInt($(this).attr("data-id")));
            }), t;
        },
        ShowLoader: function(t) {
            var e = $("#circularG");
            e.show(), e.css("margin-left", "100px"), $(t).parent().append(e);
        },
        HideLoader: function() {
            $("#circularG").hide();
        }
    }
};