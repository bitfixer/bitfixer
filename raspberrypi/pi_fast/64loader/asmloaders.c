void load_image() {
asm("ldx #$00");
asm("lda %w", 0xDD00);
asm("tay");
loop0:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait0:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait0);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2000);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_0:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_0);
asm("inx");
asm("bne %g", loop0);

loop1:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait1:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait1);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2100);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_1:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_1);
asm("inx");
asm("bne %g", loop1);

loop2:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait2:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2200);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_2:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_2);
asm("inx");
asm("bne %g", loop2);

loop3:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait3:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait3);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2300);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_3:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_3);
asm("inx");
asm("bne %g", loop3);

loop4:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait4:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait4);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2400);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_4:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_4);
asm("inx");
asm("bne %g", loop4);

loop5:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait5:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait5);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2500);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_5:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_5);
asm("inx");
asm("bne %g", loop5);

loop6:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait6:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait6);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2600);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_6:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_6);
asm("inx");
asm("bne %g", loop6);

loop7:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait7:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait7);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2700);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_7:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_7);
asm("inx");
asm("bne %g", loop7);

loop8:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait8:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait8);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2800);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_8:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_8);
asm("inx");
asm("bne %g", loop8);

loop9:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait9:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait9);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2900);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_9:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_9);
asm("inx");
asm("bne %g", loop9);

loop10:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait10:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait10);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2A00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_10:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_10);
asm("inx");
asm("bne %g", loop10);

loop11:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait11:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait11);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2B00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_11:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_11);
asm("inx");
asm("bne %g", loop11);

loop12:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait12:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait12);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2C00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_12:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_12);
asm("inx");
asm("bne %g", loop12);

loop13:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait13:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait13);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2D00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_13:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_13);
asm("inx");
asm("bne %g", loop13);

loop14:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait14:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait14);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2E00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_14:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_14);
asm("inx");
asm("bne %g", loop14);

loop15:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait15:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait15);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x2F00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_15:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_15);
asm("inx");
asm("bne %g", loop15);

loop16:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait16:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait16);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3000);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_16:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_16);
asm("inx");
asm("bne %g", loop16);

loop17:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait17:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait17);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3100);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_17:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_17);
asm("inx");
asm("bne %g", loop17);

loop18:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait18:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait18);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3200);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_18:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_18);
asm("inx");
asm("bne %g", loop18);

loop19:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait19:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait19);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3300);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_19:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_19);
asm("inx");
asm("bne %g", loop19);

loop20:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait20:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait20);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3400);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_20:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_20);
asm("inx");
asm("bne %g", loop20);

loop21:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait21:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait21);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3500);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_21:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_21);
asm("inx");
asm("bne %g", loop21);

loop22:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait22:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait22);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3600);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_22:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_22);
asm("inx");
asm("bne %g", loop22);

loop23:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait23:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait23);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3700);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_23:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_23);
asm("inx");
asm("bne %g", loop23);

loop24:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait24:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait24);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3800);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_24:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_24);
asm("inx");
asm("bne %g", loop24);

loop25:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait25:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait25);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3900);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_25:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_25);
asm("inx");
asm("bne %g", loop25);

loop26:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait26:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait26);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3A00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_26:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_26);
asm("inx");
asm("bne %g", loop26);

loop27:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait27:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait27);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3B00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_27:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_27);
asm("inx");
asm("bne %g", loop27);

loop28:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait28:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait28);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3C00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_28:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_28);
asm("inx");
asm("bne %g", loop28);

loop29:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait29:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait29);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3D00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_29:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_29);
asm("inx");
asm("bne %g", loop29);

loop30:
asm("tya");
asm("and #$FB");
asm("sta %w", 0xDD00);
wait30:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait30);
asm("lda %w", 0xDD01);
asm("sta %w,x", 0x3E00);
asm("tya");
asm("ora #$04");
asm("sta %w", 0xDD00);
wait2_30:
asm("lda %w", 0xDD0D);
asm("and #$10");
asm("beq %g", wait2_30);
asm("inx");
asm("bne %g", loop30);

}
