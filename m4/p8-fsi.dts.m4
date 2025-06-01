/dts-v1/;

/ {
	#address-cells = <0x1>;
	#size-cells = <0x0>;

	fsi@0 {
		#address-cells = <0x2>;
		#size-cells = <0x1>;
		compatible = "ibm,bmcfsi";
		reg = <0x0 0x0 0x0>;

		/* GPIO pin definitions */
		fsi_clk = <0x0 0x4>;		/* A4 */
		fsi_dat = <0x0 0x5>; 		/* A5 */
		fsi_dat_en = <0x20 0x1e>;	/* H6 */
		fsi_enable = <0x0 0x18>;	/* D0 */
		cronus_sel = <0x0 0x6>;		/* A6 */
		clock_delay = <0x14>;

		index = <0x0>;
		status = "mustexist";
		system-path = "/proc0/fsi";

		pib@1000 {
			#address-cells = <0x2>;
			#size-cells = <0x1>;
			reg = <0x0 0x1000 0x7>;
			compatible = "ibm,fsi-pib";
			index = <0x0>;
			system-path = "/proc0/pib";
		};

		hmfsi@100000 {
			#address-cells = <0x2>;
			#size-cells = <0x1>;
			compatible = "ibm,fsi-hmfsi";
			reg = <0x0 0x100000 0x8000>;
			port = <0x1>;
			index = <0x1>;
			system-path = "/proc1/fsi";

			pib@1000 {
				#address-cells = <0x2>;
				#size-cells = <0x1>;
				reg = <0x0 0x1000 0x7>;
				compatible = "ibm,fsi-pib";
				index = <0x1>;
				system-path = "/proc1/pib";
			};

		};

		hmfsi@180000 {
			#address-cells = <0x2>;
			#size-cells = <0x1>;
			compatible = "ibm,fsi-hmfsi";
			reg = <0x0 0x180000 0x80000>;
			port = <0x2>;
			index = <0x2>;
			system-path = "/proc2/fsi";

			pib@1000 {
				#address-cells = <0x2>;
				#size-cells = <0x1>;
				reg = <0x0 0x1000 0x7>;
				compatible = "ibm,fsi-pib";
				index = <0x2>;
				system-path = "/proc2/pib";
			};
		};
	};
};
