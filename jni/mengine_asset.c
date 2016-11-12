static u32
AssetManagerLoadBitmap(platform_api * Platform, 
						asset_manager * Manager){
	u32 BitmapIndex = -1;

	asset_bitmap TestBitmap = {0, 0, 0};
	TestBitmap.Width = 16;
	TestBitmap.Height = 16;
	
	TestBitmap.Data = Platform->AllocateMemory(sizeof(u32) * 16 * 16);
	/* NOTE(furkan) : Bitmap color format that OpenGL 
		expects is 0xAABBGGRR.
		Also, values below are premultiplied. Do not forget to multiply
		R,G,B with A value while importing bitmaps 	
	*/
	TestBitmap.Data[0] = 0x00000000;
	TestBitmap.Data[1] = 0x00000000;
	TestBitmap.Data[2] = 0x00000000;
	TestBitmap.Data[3] = 0x00000000;
	TestBitmap.Data[4] = 0x41323232;
	TestBitmap.Data[5] = 0x83474747;
	TestBitmap.Data[6] = 0xb9424242;
	TestBitmap.Data[7] = 0xdf313131;
	TestBitmap.Data[8] = 0xdf313131;
	TestBitmap.Data[9] = 0xb9424242;
	TestBitmap.Data[10] = 0x83474747;
	TestBitmap.Data[11] = 0x41323232;
	TestBitmap.Data[12] = 0x00000000;
	TestBitmap.Data[13] = 0x00000000;
	TestBitmap.Data[14] = 0x00000000;
	TestBitmap.Data[15] = 0x00000000;
	TestBitmap.Data[16] = 0x00000000;
	TestBitmap.Data[17] = 0x00000000;
	TestBitmap.Data[18] = 0x201c1c1c;
	TestBitmap.Data[19] = 0x8c474747;
	TestBitmap.Data[20] = 0xf3262626;
	TestBitmap.Data[21] = 0xff1d1d1d;
	TestBitmap.Data[22] = 0xff1d1d1d;
	TestBitmap.Data[23] = 0xff1d1d1d;
	TestBitmap.Data[24] = 0xff1d1d1d;
	TestBitmap.Data[25] = 0xff1d1d1d;
	TestBitmap.Data[26] = 0xff1d1d1d;
	TestBitmap.Data[27] = 0xf3262625;
	TestBitmap.Data[28] = 0x8c474748;
	TestBitmap.Data[29] = 0x201c1c1c;
	TestBitmap.Data[30] = 0x00000000;
	TestBitmap.Data[31] = 0x00000000;
	TestBitmap.Data[32] = 0x00000000;
	TestBitmap.Data[33] = 0x201c1c1c;
	TestBitmap.Data[34] = 0xa3464646;
	TestBitmap.Data[35] = 0xff1d1d1d;
	TestBitmap.Data[36] = 0xff1d1d1d;
	TestBitmap.Data[37] = 0xff1d1d1d;
	TestBitmap.Data[38] = 0xff1d1d1d;
	TestBitmap.Data[39] = 0xff1d1d1d;
	TestBitmap.Data[40] = 0xff1d1d1d;
	TestBitmap.Data[41] = 0xff1d1d1d;
	TestBitmap.Data[42] = 0xff1d1d1c;
	TestBitmap.Data[43] = 0xff1e1c22;
	TestBitmap.Data[44] = 0xff301672;
	TestBitmap.Data[45] = 0xa3464648;
	TestBitmap.Data[46] = 0x201c1c1c;
	TestBitmap.Data[47] = 0x00000000;
	TestBitmap.Data[48] = 0x00000000;
	TestBitmap.Data[49] = 0x8c474747;
	TestBitmap.Data[50] = 0xff1d1d1d;
	TestBitmap.Data[51] = 0xff1d1d1d;
	TestBitmap.Data[52] = 0xff1d1d1d;
	TestBitmap.Data[53] = 0xff1d1d1d;
	TestBitmap.Data[54] = 0xff1d1d1d;
	TestBitmap.Data[55] = 0xff1d1d1d;
	TestBitmap.Data[56] = 0xff1d1d1d;
	TestBitmap.Data[57] = 0xff1d1d1c;
	TestBitmap.Data[58] = 0xff1c1d18;
	TestBitmap.Data[59] = 0xff1c1d18;
	TestBitmap.Data[60] = 0xff381397;
	TestBitmap.Data[61] = 0xff301674;
	TestBitmap.Data[62] = 0x8c474747;
	TestBitmap.Data[63] = 0x00000000;
	TestBitmap.Data[64] = 0x41323232;
	TestBitmap.Data[65] = 0xf3262625;
	TestBitmap.Data[66] = 0xff1d1d1c;
	TestBitmap.Data[67] = 0xff1d1d1d;
	TestBitmap.Data[68] = 0xff1d1d1d;
	TestBitmap.Data[69] = 0xff1d1d1d;
	TestBitmap.Data[70] = 0xff1d1d1d;
	TestBitmap.Data[71] = 0xff1d1d1d;
	TestBitmap.Data[72] = 0xff1d1d1b;
	TestBitmap.Data[73] = 0xff1d1d1d;
	TestBitmap.Data[74] = 0xff201c2c;
	TestBitmap.Data[75] = 0xff251a41;
	TestBitmap.Data[76] = 0xff39129b;
	TestBitmap.Data[77] = 0xff4010bd;
	TestBitmap.Data[78] = 0xf331215a;
	TestBitmap.Data[79] = 0x41323231;
	TestBitmap.Data[80] = 0x83464746;
	TestBitmap.Data[81] = 0xff231b36;
	TestBitmap.Data[82] = 0xff1d1d1d;
	TestBitmap.Data[83] = 0xff1d1d1c;
	TestBitmap.Data[84] = 0xff1d1d1d;
	TestBitmap.Data[85] = 0xff1d1d1d;
	TestBitmap.Data[86] = 0xff1d1d1d;
	TestBitmap.Data[87] = 0xff1d1d1c;
	TestBitmap.Data[88] = 0xff1b1e16;
	TestBitmap.Data[89] = 0xff311678;
	TestBitmap.Data[90] = 0xff3c11ac;
	TestBitmap.Data[91] = 0xff3e11b1;
	TestBitmap.Data[92] = 0xff3e11b1;
	TestBitmap.Data[93] = 0xff3f10b9;
	TestBitmap.Data[94] = 0xff2c1764;
	TestBitmap.Data[95] = 0x83464744;
	TestBitmap.Data[96] = 0xb941423e;
	TestBitmap.Data[97] = 0xff281851;
	TestBitmap.Data[98] = 0xff291854;
	TestBitmap.Data[99] = 0xff1a1e11;
	TestBitmap.Data[100] = 0xff1c1e17;
	TestBitmap.Data[101] = 0xff1c1d19;
	TestBitmap.Data[102] = 0xff1b1e16;
	TestBitmap.Data[103] = 0xff1a1e12;
	TestBitmap.Data[104] = 0xff281952;
	TestBitmap.Data[105] = 0xff3e11b3;
	TestBitmap.Data[106] = 0xff3e11b1;
	TestBitmap.Data[107] = 0xff3d11af;
	TestBitmap.Data[108] = 0xff3e10b3;
	TestBitmap.Data[109] = 0xff36148e;
	TestBitmap.Data[110] = 0xff1e1c22;
	TestBitmap.Data[111] = 0xb9414241;
	TestBitmap.Data[112] = 0xdf303130;
	TestBitmap.Data[113] = 0xff1f1c26;
	TestBitmap.Data[114] = 0xff39139b;
	TestBitmap.Data[115] = 0xff301574;
	TestBitmap.Data[116] = 0xff241b3b;
	TestBitmap.Data[117] = 0xff211c2d;
	TestBitmap.Data[118] = 0xff241a3d;
	TestBitmap.Data[119] = 0xff301674;
	TestBitmap.Data[120] = 0xff3d11af;
	TestBitmap.Data[121] = 0xff3d11b0;
	TestBitmap.Data[122] = 0xff3d11ae;
	TestBitmap.Data[123] = 0xff3d11b0;
	TestBitmap.Data[124] = 0xff3e11b2;
	TestBitmap.Data[125] = 0xff39129b;
	TestBitmap.Data[126] = 0xff1f1c27;
	TestBitmap.Data[127] = 0xdf303130;
	TestBitmap.Data[128] = 0xdf313131;
	TestBitmap.Data[129] = 0xff1b1e16;
	TestBitmap.Data[130] = 0xff251a43;
	TestBitmap.Data[131] = 0xff3e10b2;
	TestBitmap.Data[132] = 0xff3f10b9;
	TestBitmap.Data[133] = 0xff3c11aa;
	TestBitmap.Data[134] = 0xff3e11b1;
	TestBitmap.Data[135] = 0xff3f10b6;
	TestBitmap.Data[136] = 0xff3d11b0;
	TestBitmap.Data[137] = 0xff3e11b2;
	TestBitmap.Data[138] = 0xff3e10b3;
	TestBitmap.Data[139] = 0xff3b12a6;
	TestBitmap.Data[140] = 0xff3d11ae;
	TestBitmap.Data[141] = 0xff3f10b7;
	TestBitmap.Data[142] = 0xff2a1859;
	TestBitmap.Data[143] = 0xdf30322b;
	TestBitmap.Data[144] = 0xb9424242;
	TestBitmap.Data[145] = 0xff1d1d1d;
	TestBitmap.Data[146] = 0xff1b1e16;
	TestBitmap.Data[147] = 0xff221b35;
	TestBitmap.Data[148] = 0xff32157e;
	TestBitmap.Data[149] = 0xff3c11aa;
	TestBitmap.Data[150] = 0xff3e10b4;
	TestBitmap.Data[151] = 0xff3e10b4;
	TestBitmap.Data[152] = 0xff3d11b1;
	TestBitmap.Data[153] = 0xff39129c;
	TestBitmap.Data[154] = 0xff291854;
	TestBitmap.Data[155] = 0xff32157a;
	TestBitmap.Data[156] = 0xff3e10b4;
	TestBitmap.Data[157] = 0xff3f10b7;
	TestBitmap.Data[158] = 0xff2f176d;
	TestBitmap.Data[159] = 0xb941423d;
	TestBitmap.Data[160] = 0x83474747;
	TestBitmap.Data[161] = 0xff1d1d1d;
	TestBitmap.Data[162] = 0xff1d1d1d;
	TestBitmap.Data[163] = 0xff1c1d1b;
	TestBitmap.Data[164] = 0xff1f1c25;
	TestBitmap.Data[165] = 0xff1f1c25;
	TestBitmap.Data[166] = 0xff241a3b;
	TestBitmap.Data[167] = 0xff251a42;
	TestBitmap.Data[168] = 0xff221b33;
	TestBitmap.Data[169] = 0xff1c1d1b;
	TestBitmap.Data[170] = 0xff261948;
	TestBitmap.Data[171] = 0xff3c11ab;
	TestBitmap.Data[172] = 0xff3d11af;
	TestBitmap.Data[173] = 0xff3f10b7;
	TestBitmap.Data[174] = 0xff2b185b;
	TestBitmap.Data[175] = 0x83464744;
	TestBitmap.Data[176] = 0x41323232;
	TestBitmap.Data[177] = 0xf3262626;
	TestBitmap.Data[178] = 0xff1d1d1d;
	TestBitmap.Data[179] = 0xff1c1d19;
	TestBitmap.Data[180] = 0xff2d1765;
	TestBitmap.Data[181] = 0xff31157a;
	TestBitmap.Data[182] = 0xff271949;
	TestBitmap.Data[183] = 0xff251a42;
	TestBitmap.Data[184] = 0xff291853;
	TestBitmap.Data[185] = 0xff331482;
	TestBitmap.Data[186] = 0xff3e11b1;
	TestBitmap.Data[187] = 0xff3d11b0;
	TestBitmap.Data[188] = 0xff3e11b4;
	TestBitmap.Data[189] = 0xff3b12a4;
	TestBitmap.Data[190] = 0xf3282531;
	TestBitmap.Data[191] = 0x41323231;
	TestBitmap.Data[192] = 0x00000000;
	TestBitmap.Data[193] = 0x8c474747;
	TestBitmap.Data[194] = 0xff1d1d1d;
	TestBitmap.Data[195] = 0xff1c1d1a;
	TestBitmap.Data[196] = 0xff1f1c24;
	TestBitmap.Data[197] = 0xff371391;
	TestBitmap.Data[198] = 0xff410fc0;
	TestBitmap.Data[199] = 0xff3f10b9;
	TestBitmap.Data[200] = 0xff3f10b8;
	TestBitmap.Data[201] = 0xff3f10b7;
	TestBitmap.Data[202] = 0xff3e11b2;
	TestBitmap.Data[203] = 0xff3f10b7;
	TestBitmap.Data[204] = 0xff3d11ac;
	TestBitmap.Data[205] = 0xff271949;
	TestBitmap.Data[206] = 0x8c474745;
	TestBitmap.Data[207] = 0x00000000;
	TestBitmap.Data[208] = 0x00000000;
	TestBitmap.Data[209] = 0x201c1c1c;
	TestBitmap.Data[210] = 0xa3464646;
	TestBitmap.Data[211] = 0xff1d1d1d;
	TestBitmap.Data[212] = 0xff1c1d19;
	TestBitmap.Data[213] = 0xff1d1d1f;
	TestBitmap.Data[214] = 0xff2b1760;
	TestBitmap.Data[215] = 0xff371396;
	TestBitmap.Data[216] = 0xff3c11a8;
	TestBitmap.Data[217] = 0xff3c11aa;
	TestBitmap.Data[218] = 0xff3a12a1;
	TestBitmap.Data[219] = 0xff32157b;
	TestBitmap.Data[220] = 0xff221b36;
	TestBitmap.Data[221] = 0xa3454643;
	TestBitmap.Data[222] = 0x201c1c1c;
	TestBitmap.Data[223] = 0x00000000;
	TestBitmap.Data[224] = 0x00000000;
	TestBitmap.Data[225] = 0x00000000;
	TestBitmap.Data[226] = 0x201c1c1c;
	TestBitmap.Data[227] = 0x8c474747;
	TestBitmap.Data[228] = 0xf3262626;
	TestBitmap.Data[229] = 0xff1d1d1b;
	TestBitmap.Data[230] = 0xff1b1e15;
	TestBitmap.Data[231] = 0xff1d1d1e;
	TestBitmap.Data[232] = 0xff211b2e;
	TestBitmap.Data[233] = 0xff211b32;
	TestBitmap.Data[234] = 0xff1f1c25;
	TestBitmap.Data[235] = 0xf325261f;
	TestBitmap.Data[236] = 0x8c474746;
	TestBitmap.Data[237] = 0x201c1c1c;
	TestBitmap.Data[238] = 0x00000000;
	TestBitmap.Data[239] = 0x00000000;
	TestBitmap.Data[240] = 0x00000000;
	TestBitmap.Data[241] = 0x00000000;
	TestBitmap.Data[242] = 0x00000000;
	TestBitmap.Data[243] = 0x00000000;
	TestBitmap.Data[244] = 0x41323232;
	TestBitmap.Data[245] = 0x83474747;
	TestBitmap.Data[246] = 0xb9424242;
	TestBitmap.Data[247] = 0xdf313130;
	TestBitmap.Data[248] = 0xdf30312f;
	TestBitmap.Data[249] = 0xb941423f;
	TestBitmap.Data[250] = 0x83464746;
	TestBitmap.Data[251] = 0x41323232;
	TestBitmap.Data[252] = 0x00000000;
	TestBitmap.Data[253] = 0x00000000;
	TestBitmap.Data[254] = 0x00000000;
	TestBitmap.Data[255] = 0x00000000;

	/* TODO(furkan) : Bounds checking */
	BitmapIndex = Manager->BitmapCount++;
	Manager->Bitmaps[BitmapIndex] = TestBitmap;

	return BitmapIndex;
}
