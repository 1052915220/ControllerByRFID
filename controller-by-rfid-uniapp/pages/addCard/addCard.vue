<template>
	<view>
		<u-toast ref="uToast"></u-toast>

		<!-- 弹出层开始 -->
		<view>
			<u-popup :round="20" :closeOnClickOverlay="true" :show="popupShow" @close="close" mode="center">
				<view
					style="width: 500rpx; height: 250rpx; display: flex;align-items: center; justify-content: center;">
					<view>
						<view style="width: 300rpx;margin: 0 auto;">
							<u--input placeholder="拥有者" border="surround" v-model="value"></u--input>
						</view>
						<u-button style="margin-top: 15px; width: 200rpx;" shape="circle" type="primary" text="确定"
							@click="addCard"></u-button>
					</view>
				</view>
			</u-popup>
		</view>
		<!-- 弹出层结束 -->

		<!-- 确认清空模态框开始 -->
		<view>
			<u-modal @cancel="modalShow=false" @confirm="clearConfig" :showCancelButton="true" :show="modalShow"
				title="警告" content='即将清空设备与本地的数据,请谨慎处理.'></u-modal>
		</view>
		<!-- 确认清空模态框结束 -->

		<!-- 		<view style="height: 300rpx;padding: 30rpx;">
			<u--text style="justify-content: center;margin-top: 50rpx" :type="type" :text="statusText"></u--text>
		</view> -->

		<view>
			<u-alert title="说明" type = "primary " description = "descripntion"></u-alert>
		</view>

		<view style="margin: 80rpx 0 50rpx 0;">
			<u-button @click="readyAdd" shape="circle" style="height: 70rpx;width: 400rpx;" type="primary"
				text="添加卡片"></u-button>
		</view>
		<view style="display: inline-block; float: right; width: 110rpx">
			<span style="display: flex; justify-content: flex-end; align-items: center; width: 100%">
				<u-button @click="startClear" type="warning" size="mini" text="清空配置" style="flex-shrink: 0"></u-button>
			</span>
		</view>
		<view class="uni-container">
			<uni-table ref="table" :loading="tableLoading" emptyText="暂无更多数据,请添加卡片">
				<uni-tr>
					<uni-th width="10" align="center">序号</uni-th>
					<uni-th width="50" align="center">拥有者</uni-th>
					<uni-th width="40" align="center">卡片</uni-th>
					<uni-th width="20" align="center">设置</uni-th>
				</uni-tr>
				<uni-tr v-for="(item, index) in tableData" :key="index">
					<uni-td align="center">{{ index+1 }}</uni-td>
					<uni-td align="center">{{ item.name }}</uni-td>
					<uni-td align="center">{{ item.cardUUID }}</uni-td>
					<uni-td>
						<view class="uni-group">
							<button class="uni-button" size="mini" type="warn" @click="deleteCard(index+1)">删除</button>
						</view>
					</uni-td>
				</uni-tr>
			</uni-table>
		</view>
		<view style="margin-top: 10rpx;">
			<u-button :loading="buttonLoading" loadingText="刷新中" shape="success" text="刷新" @click="refresh"></u-button>
		</view>
		<u-loading-page iconSize="30" :loading="loading" bg-color="rgba(0, 0, 0, 0.5)"></u-loading-page>
	</view>
</template>

<script>
	import list from '../../uni_modules/uview-ui/libs/config/props/list';
	export default {
		data() {
			return {
				modalShow: false,
				statusText: "设备未连接",
				type: "error",
				value: "",
				popupShow: false,
				buttonLoading: false,
				show: false,
				tableData: [],
				tableLoading: false,
				loading: false,
				list: [{
						type: 'error',
						title: '失败',
						message: "失败",
						iconUrl: 'https://cdn.uviewui.com/uview/demo/toast/error.png'
					},
					{
						type: 'success',
						title: '成功',
						message: "成功",
						iconUrl: 'https://cdn.uviewui.com/uview/demo/toast/success.png'
					}
				],
			}
		},
		onLoad() {
			try {
				console.log("开始获取缓存");
				const value = uni.getStorageSync('tableData');
				if (value) {
					console.log(value);
				}
				this.tableData = JSON.parse(value);
			} catch (e) {
				console.log("获取缓存失败");
			}

			/* 			uni.startWifi({
							success: () => {
								console.log("初始化成功");
							},
							fail: () => {
								console.log("初始化失败");
							}
						});
						uni.onWifiConnected(res => {
							console.log('Wi-Fi 连接成功，Wi-Fi 信息为：', res)
						});
						uni.getConnectedWifi({
							partialInfo: false,
							success: () => {
								this.connectionInfo("1");
								// if (wifi.startsWith("CloudLight")) {

								// }
							},
							fail: () => {
								this.connectionInfo("0");
							}
						}); */
		},
		computed: {
			getIcon() {
				return path => {
					return 'https://cdn.uviewui.com/uview/example/' + path + '.png';
				}
			},
		},
		methods: {
			connectionInfo(str) {
				if (str == "1") {
					console.log("连接成功");
					this.statusText = "连接正常，已准备就绪";
					this.type = "success";
				} else {
					console.log("连接不成功");
					this.statusText = "设备连接失败，请连接[CloudLight-xxxxx]-WiFi";
					this.type = "error";
				}
			},
			close() {
				this.popupShow = false
				console.log('close');
			},
			//刷新
			refresh() {
				console.log("点击刷新");
				this.buttonLoading = true;
				let url = getApp().globalData.url + "query";
				console.log("开始访问", url);
				uni.request({
					method: "GET",
					url: url,
					timeout: 2000,
					success: (res) => {
						let data = res.data;
						console.log(data);
						let cardUUIDs = data.cardUUIDs;
						console.log(data.cardUUIDs);
						for (let i = 0; i < cardUUIDs.length; i++) {
							if (this.tableData[i] == null) {
								break;
							}
							this.tableData[i].cardUUID = cardUUIDs[i];
						}
						try {
							console.log("保存至本地缓存");
							uni.setStorageSync('tableData', JSON.stringify(this.tableData));
						} catch (e) {
							console.log("保存至本地缓存出错");
						}

						this.buttonLoading = false;
					},
					fail: () => {
						console.log("请求失败");
						this.buttonLoading = false;
						this.showToast(this.list[0]);
					}
				});
			},
			//删除
			deleteCard(id) {
				console.log("点击删除");
				this.loading = true;
				uni.request({
					method: "GET",
					url: getApp().globalData.url + "delete",
					timeout: 2000,
					data: {
						id: id
					},
					success: (res) => {
						console.log(res.data);
						this.tableData.splice(id-1,1);
						this.loading = false;
						this.refresh();
						this.list[1].message = res.data;
						this.showToast(this.list[1]);
					},
					fail: () => {
						this.loading = false;
						this.showToast(this.list[0]);
					}
				});
			},
			readyAdd() {
				console.log("点击添加卡片");
				this.popupShow = true;
			},
			//添加
			addCard() {
				console.log("点击添加卡片确认");
				console.log(this.value);
				if (this.tableData.length > 7) {
					uni.showToast({
						title: '卡片最大数量为7', // 消息内容
						icon: 'none', // 消息图标，可选值：success/loading/none
						duration: 1000 // 持续时间(ms)，默认值2000(2秒)
					});
					return
				}
				if (this.value.trim() == "") {
					console.log("123123");
					uni.showToast({
						title: '不能为空', // 消息内容
						icon: 'none', // 消息图标，可选值：success/loading/none
						duration: 1000 // 持续时间(ms)，默认值2000(2秒)
					});
					return;
				}
				let cardEmty = {
					"date": (this.list[this.list.length] + 1) + "",
					"name": this.value,
					"cardUUID": "",
				};
				this.popupShow = false;
				this.loading = true;
				uni.request({
					method: "GET",
					url: getApp().globalData.url + "add",
					timeout: 2000,
					success: (res) => {
						console.log(res.data);
						this.tableData.push(cardEmty);
						this.loading = false;
						this.list[1].message = res.data;
						this.showToast(this.list[1]);
						this.refresh();
					},
					fail: () => {
						this.loading = false;
						this.showToast(this.list[0]);
					}
				});
			},
			startClear() {
				this.modalShow = true;
			},
			//清空配置
			clearConfig() {
				console.log("点击添加卡片");
				this.modalShow = false;
				this.loading = true;
				uni.request({
					method: "GET",
					url: getApp().globalData.url + "clear",
					timeout: 2000,
					success: (res) => {
						console.log(res.data);
						this.loading = false;

						try {
							console.log("清空本地缓存");
							uni.clearStorageSync("tableData");
						} catch (e) {
							console.log("清空本地缓存失败");
						}
						this.list[1].message = res.data;
						this.showToast(this.list[1]);
					},
					fail: () => {
						this.loading = false;
						this.showToast(this.list[0]);
					}
				});
			},
			showToast(params) {
				this.$refs.uToast.show({
					...params,
					complete() {
						params.url && uni.navigateTo({

							url: params.url
						})
					}
				})
			}


		}
	}
</script>

<style>
	.uni-group {
		display: flex;
		align-items: center;
	}

	.uni-table-td[data-v-321f8e79] {
		padding: 0;
	}

	.u-page {
		padding: 0;
	}

	.u-cell-icon {
		width: 36rpx;
		height: 36rpx;
		margin-right: 8rpx;
	}

	.u-cell-group__title__text {
		font-weight: bold;
	}
</style>