/************************************************
 * http://www.jq-school.com/Article.aspx?kid=471/
 * @author :leon.z
 * @email  :LeonJsObj@126.com
 * @date   :2014-9-3
 * @description :自己写的alert和confirm弹出框
 ***********************************************/
/**
	自己写的alert和confirm弹出框。用法很简单，直接调用插件对象传人参数ok,
	注意的是自己无聊将插件升级为用户自定义，完全根据用户审美自定义颜色，再就是有一个全局颜色随机机制，如果您不是重口味，建议您不要开启,
	自动机制也可以用于调试自己喜欢颜色，然后取到配色方案再关闭全局自动，附上自己找到配色并赋值
	eg:$.MsgBox({type:"confirm",speed:200,title:"",msg:"",opacity:'',callback:function(){}.....})
**/
;(function($) {
	$.MsgBox = function(options) {
		$.MsgBox.defaults = {
			is_changeBg: true, //默认显示遮罩层
			title: '消息', //头部
			msg: '至少来点内容吧！', //内容
			opacity: .5, //遮罩层透明度
			bgColor: '#000', //遮罩层颜色
			type: '', //调用类型（必须填写）
			userDesign: false, //用户自定义（默认false）
			borderColor: "#009bfe", //title下边框颜色(只针对自定义下有效)
			titleBg: "#ddd", //title背景颜色(只针对自定义下有效)
			contentBg: "#fff", //内容背景(只针对自定义下有效)
			dashColor: "#ddd", //虚线颜色(只针对自定义下有效)
			btnBg: "#168bbb", //按钮颜色(只针对自定义下有效)
			iconHoverBg: "red", //关闭按钮放上去颜色(只针对自定义下有效)
			AllColorAuto: false, //是否启用全局颜色随机产生，启用后自定义的颜色将实效(只针对自定义下有效)
			callback: function() {} //声明回调函数
		};
		var opts = $.extend(true, $.MsgBox.defaults, options);
		if (!!opts.userDesign && opts.userDesign == !0) {
			if (!!opts.AllColorAuto && opts.AllColorAuto == !0) {
				var btnBg = '#' + (Math.random() * 0xffffff << 0).toString(16);
				var borderColor = '#' + (Math.random() * 0xffffff << 0).toString(16);
				var titleBg = '#ddd' //不敢随机，影响食欲
				var contentBg = '#fff'; //不敢随机，影响食欲
				var dashColor = '#' + (Math.random() * 0xffffff << 0).toString(16);
				var iconHoverBg = '#' + (Math.random() * 0xffffff << 0).toString(16);
			} else {
				var btnBg = opts.btnBg;
				var borderColor = opts.borderColor;
				var titleBg = opts.titleBg;
				var contentBg = opts.contentBg;
				var dashColor = opts.dashColor;
				var iconHoverBg = opts.iconHoverBg;
			}
		} else {
			var borderColor = "#009bfe";
			var titleBg = "#ddd";
			var contentBg = "#fff";
			var dashColor = "#ddd";
			var btnBg = "#168bbb";
			var iconHoverBg = "red";
		}
		(function(opts) {
			switch (opts.type) { //检查用户传进来的类型，目前只开发到alert,confirm两种
				case "alert":
					pageRender("alert", opts.title, opts.msg);
					btnOk();
					btnNo();
					break;
				case "confirm":
					pageRender("confirm", opts.title, opts.msg);
					btnOk(opts.callback);
					btnNo();
					break;
				default:
					pageRender("alert", '消息', "暂时未开发其他功能，请持续关注leon.z插件库");
					btnOk();
					btnNo();
					break;
			}
		})(opts);
		function pageRender(type, title, msg) { /** 渲染页面**/
			var html = "";
			html += '<div id="msg_box"></div><div id="msg_con"><span id="msg_tit">' + title + '</span>';
			html += '<a id="msg_ico">x</a><div id="msg_msg">' + msg + '</div><div id="msg_btnbox">';
			if (type == "alert") {
				html += '<input id="msg_btn_ok" type="button" value="确定" />';
			}
			if (type == "confirm") {
				html += '<input id="msg_btn_ok" type="button" value="确定" />';
				html += '<input id="msg_btn_no" type="button" value="取消" />';
			}
			html += '</div></div>';
			//必须先将html添加到body，再设置Css样式
			$("body").append(html);
			renderCss(); //渲染样式
		};
		function renderCss() {
				var msgBox = document.body.clientWidth > 400 ? 400 : document.body.clientWidth - 10;
				if (opts.is_changeBg == !0) {
					$("#msg_box").css({
						width: '100%',
						height: '100%',
						zIndex: '99999',
						position: 'fixed',
						backgroundColor: opts.bgColor,
						top: '0',
						left: '0',
						opacity: opts.opacity
					});
				} else {
					$("#msg_con").css({
						border: "1px solid " + borderColor,
						boxShadow: "0px 2px 0px " + borderColor
					});
				}
				$("#msg_con").css({
					zIndex: '999999',
					width: msgBox + 'px',
					position: 'fixed',
					backgroundColor: contentBg,
					borderRadius: '15px',
					cursor: 'pointer'
				});

				$("#msg_tit").css({
					display: 'block',
					fontSize: '14px',
					color: '#444',
					padding: '10px 15px',
					backgroundColor: titleBg,
					borderRadius: '15px 15px 0 0',
					borderBottom: '3px solid ' + borderColor,
					fontWeight: 'bold'
				});

				$("#msg_msg").css({
					padding: '20px',
					lineHeight: '20px',
					borderBottom: '1px dashed ' + dashColor,
					fontSize: '13px'
				});

				$("#msg_ico").css({
					display: 'block',
					position: 'absolute',
					right: '10px',
					top: '9px',
					border: '1px solid Gray',
					width: '18px',
					height: '18px',
					textAlign: 'center',
					lineHeight: '15px',
					cursor: 'pointer',
					borderRadius: '12px',
					fontFamily: '微软雅黑'
				});
				$("#msg_btnbox").css({
					margin: '15px 0 10px 0',
					textAlign: 'center'
				});
				$("#msg_btn_ok,#msg_btn_no").css({
					width: '85px',
					height: '30px',
					color: 'white',
					border: 'none',
					cursor: 'pointer'
				});
				$("#msg_btn_ok").css({
					backgroundColor: btnBg
				});
				$("#msg_btn_no").css({
					backgroundColor: 'gray',
					marginLeft: '20px'
				});
				//右上角关闭按钮hover样式
				$("#msg_ico").hover(function() {
					var hoverClass = {
						"transition-property": "all",
						"transition-duration": "0.3s",
						"transition-timing-function": "ease-in",
						"transition-delay": "0.05s",
						"backgroundColor": iconHoverBg,
						"color": "white"
					}
					if (window.applicationCache) { //是否支持html5 transition属性
						$(this).css(hoverClass || {});
					} else {
						$(this).css({
							backgroundColor: iconHoverBg,
							color: 'white'
						});
					}
				}, function() {
					$(this).css({
						backgroundColor: titleBg,
						color: 'black'
					});
				});
				var _widht = window.screen.clientWidth || document.documentElement.clientWidth; //屏幕宽
				var _height = window.screen.clientHeight || document.documentElement.clientHeight; //屏幕高
				var msgBoxidth = $("#msg_con").width();
				var boxHeight = $("#msg_con").height() + 100;
				//让提示框居中
				$("#msg_con").css({
					top: (_height - boxHeight) / 2 + "px",
					left: (_widht - msgBoxidth) / 2 + "px"
				});
			}
		//确定按钮事件
		function btnOk(callback) {
				$("#msg_btn_ok").click(function() {
					$("#msg_box,#msg_con").remove();
					if (typeof(callback) == 'function') {
						callback();
					}
				});
			}
		//取消按钮事件
		function btnNo() {
			$("#msg_btn_no,#msg_ico").click(function() {
				$("#msg_box,#msg_con").remove();
			});
		}
	}
})(jQuery);