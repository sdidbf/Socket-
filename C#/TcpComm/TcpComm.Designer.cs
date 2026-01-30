using System.Windows.Forms;

namespace TcpCommTest
{
    partial class TcpComm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TcpComm));
            this.label1 = new System.Windows.Forms.Label();
            this.tbxIpAddressServer = new System.Windows.Forms.TextBox();
            this.tbxIpPortServer = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.btnConnectStatusServer = new System.Windows.Forms.Button();
            this.btnConnect = new System.Windows.Forms.Button();
            this.btnDisConnect = new System.Windows.Forms.Button();
            this.btnRun = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.cbxDetectModual = new System.Windows.Forms.ComboBox();
            this.visionMessageFactoryBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.label3 = new System.Windows.Forms.Label();
            this.colResoure = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.colInfo = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.btnConnectStatusClient = new System.Windows.Forms.Button();
            this.tbxIpPortClient = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.tbxIpAddressClient = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.visionMessageFactoryBindingSource)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(28, 54);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(98, 23);
            this.label1.TabIndex = 0;
            this.label1.Text = "IP Address:";
            // 
            // tbxIpAddressServer
            // 
            this.tbxIpAddressServer.Enabled = false;
            this.tbxIpAddressServer.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbxIpAddressServer.Location = new System.Drawing.Point(122, 51);
            this.tbxIpAddressServer.Name = "tbxIpAddressServer";
            this.tbxIpAddressServer.Size = new System.Drawing.Size(157, 28);
            this.tbxIpAddressServer.TabIndex = 1;
            this.tbxIpAddressServer.TabStop = false;
            this.tbxIpAddressServer.Text = "127.0.0.1";
            this.tbxIpAddressServer.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // tbxIpPortServer
            // 
            this.tbxIpPortServer.Enabled = false;
            this.tbxIpPortServer.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbxIpPortServer.Location = new System.Drawing.Point(371, 53);
            this.tbxIpPortServer.Name = "tbxIpPortServer";
            this.tbxIpPortServer.Size = new System.Drawing.Size(157, 28);
            this.tbxIpPortServer.TabIndex = 3;
            this.tbxIpPortServer.Text = "8080";
            this.tbxIpPortServer.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(302, 56);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(70, 23);
            this.label2.TabIndex = 2;
            this.label2.Text = "IP Port:";
            // 
            // btnConnectStatusServer
            // 
            this.btnConnectStatusServer.BackColor = System.Drawing.Color.Silver;
            this.btnConnectStatusServer.ForeColor = System.Drawing.Color.Transparent;
            this.btnConnectStatusServer.Location = new System.Drawing.Point(539, 48);
            this.btnConnectStatusServer.Name = "btnConnectStatusServer";
            this.btnConnectStatusServer.Size = new System.Drawing.Size(70, 38);
            this.btnConnectStatusServer.TabIndex = 4;
            this.btnConnectStatusServer.UseVisualStyleBackColor = false;
            // 
            // btnConnect
            // 
            this.btnConnect.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnConnect.Location = new System.Drawing.Point(177, 250);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(96, 31);
            this.btnConnect.TabIndex = 5;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Visible = false;
            // 
            // btnDisConnect
            // 
            this.btnDisConnect.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnDisConnect.Location = new System.Drawing.Point(289, 249);
            this.btnDisConnect.Name = "btnDisConnect";
            this.btnDisConnect.Size = new System.Drawing.Size(96, 31);
            this.btnDisConnect.TabIndex = 6;
            this.btnDisConnect.Text = "DisConnect";
            this.btnDisConnect.UseVisualStyleBackColor = true;
            this.btnDisConnect.Visible = false;
            // 
            // btnRun
            // 
            this.btnRun.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnRun.Location = new System.Drawing.Point(151, 310);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(109, 45);
            this.btnRun.TabIndex = 7;
            this.btnRun.Text = "Running";
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Visible = false;
            // 
            // btnStop
            // 
            this.btnStop.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnStop.Location = new System.Drawing.Point(289, 310);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(109, 45);
            this.btnStop.TabIndex = 8;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Visible = false;
            // 
            // cbxDetectModual
            // 
            this.cbxDetectModual.DataSource = this.visionMessageFactoryBindingSource;
            this.cbxDetectModual.Enabled = false;
            this.cbxDetectModual.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbxDetectModual.FormattingEnabled = true;
            this.cbxDetectModual.Location = new System.Drawing.Point(289, 196);
            this.cbxDetectModual.Name = "cbxDetectModual";
            this.cbxDetectModual.Size = new System.Drawing.Size(148, 31);
            this.cbxDetectModual.TabIndex = 9;
            // 
            // visionMessageFactoryBindingSource
            // 
            this.visionMessageFactoryBindingSource.DataSource = typeof(TcpCommTest.VisionMessageFactory);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(155, 199);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(133, 23);
            this.label3.TabIndex = 10;
            this.label3.Text = "Detect Modual:";
            // 
            // colResoure
            // 
            this.colResoure.Width = 20;
            // 
            // colInfo
            // 
            this.colInfo.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.colInfo.Width = 150;
            // 
            // btnConnectStatusClient
            // 
            this.btnConnectStatusClient.BackColor = System.Drawing.Color.Silver;
            this.btnConnectStatusClient.ForeColor = System.Drawing.Color.Transparent;
            this.btnConnectStatusClient.Location = new System.Drawing.Point(539, 133);
            this.btnConnectStatusClient.Name = "btnConnectStatusClient";
            this.btnConnectStatusClient.Size = new System.Drawing.Size(70, 38);
            this.btnConnectStatusClient.TabIndex = 15;
            this.btnConnectStatusClient.UseVisualStyleBackColor = false;
            // 
            // tbxIpPortClient
            // 
            this.tbxIpPortClient.Enabled = false;
            this.tbxIpPortClient.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbxIpPortClient.Location = new System.Drawing.Point(371, 138);
            this.tbxIpPortClient.Name = "tbxIpPortClient";
            this.tbxIpPortClient.Size = new System.Drawing.Size(157, 28);
            this.tbxIpPortClient.TabIndex = 14;
            this.tbxIpPortClient.Text = "8080";
            this.tbxIpPortClient.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(302, 141);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 23);
            this.label4.TabIndex = 13;
            this.label4.Text = "IP Port:";
            // 
            // tbxIpAddressClient
            // 
            this.tbxIpAddressClient.Enabled = false;
            this.tbxIpAddressClient.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbxIpAddressClient.Location = new System.Drawing.Point(122, 136);
            this.tbxIpAddressClient.Name = "tbxIpAddressClient";
            this.tbxIpAddressClient.Size = new System.Drawing.Size(157, 28);
            this.tbxIpAddressClient.TabIndex = 12;
            this.tbxIpAddressClient.Text = "127.0.0.1";
            this.tbxIpAddressClient.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(28, 139);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(98, 23);
            this.label5.TabIndex = 11;
            this.label5.Text = "IP Address:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(28, 20);
            this.label6.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(62, 23);
            this.label6.TabIndex = 16;
            this.label6.Text = "Server";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Trebuchet MS", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(28, 104);
            this.label7.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(58, 23);
            this.label7.TabIndex = 17;
            this.label7.Text = "Client";
            // 
            // TcpComm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(632, 362);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.btnConnectStatusClient);
            this.Controls.Add(this.tbxIpPortClient);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.tbxIpAddressClient);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.cbxDetectModual);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnRun);
            this.Controls.Add(this.btnDisConnect);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.btnConnectStatusServer);
            this.Controls.Add(this.tbxIpPortServer);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tbxIpAddressServer);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("Times New Roman", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "TcpComm";
            this.Text = "TcpComm";
            ((System.ComponentModel.ISupportInitialize)(this.visionMessageFactoryBindingSource)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbxIpAddressServer;
        private System.Windows.Forms.TextBox tbxIpPortServer;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnConnectStatusServer;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Button btnDisConnect;
        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.ComboBox cbxDetectModual;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.BindingSource visionMessageFactoryBindingSource;
        private System.Windows.Forms.ColumnHeader colResoure;
        private System.Windows.Forms.ColumnHeader colInfo;
        private Button btnConnectStatusClient;
        private TextBox tbxIpPortClient;
        private Label label4;
        private TextBox tbxIpAddressClient;
        private Label label5;
        private Label label6;
        private Label label7;
    }
}

