`timescale 1ns/1ps

module txlol_hw_engine_tb;

    localparam SAMPLE_W = 16;
    localparam ACC_W    = 64;
    localparam CFG_W    = 16;

    reg                         clk;
    reg                         rst_n;
    reg                         start;
    reg                         abort;
    reg                         sample_valid;
    reg  signed [SAMPLE_W-1:0]  tx_i;
    reg  signed [SAMPLE_W-1:0]  tx_q;
    reg  signed [SAMPLE_W-1:0]  obs_i;
    reg  signed [SAMPLE_W-1:0]  obs_q;
    reg         [CFG_W-1:0]     cfg_num_batches;
    reg         [CFG_W-1:0]     cfg_samples_per_batch;
    reg         [CFG_W-1:0]     cfg_guard_samples;
    reg         [CFG_W-1:0]     cfg_perturb_amp;
    reg                         cfg_start_sign;
    reg                         cfg_sq_mode_sample;
    wire                        busy;
    wire                        done;
    wire                        valid;
    wire                        cfg_error;
    wire signed [ACC_W-1:0]     TxDcTotalI;
    wire signed [ACC_W-1:0]     TxDcTotalQ;
    wire signed [ACC_W-1:0]     ObsDcTotalI;
    wire signed [ACC_W-1:0]     ObsDcTotalQ;
    wire signed [ACC_W-1:0]     TxPerturb;
    wire signed [ACC_W-1:0]     TxPerturbSq;
    wire signed [ACC_W-1:0]     RxDcDiffI;
    wire signed [ACC_W-1:0]     RxDcDiffQ;
    wire signed [ACC_W-1:0]     RxDcDiffSqI;
    wire signed [ACC_W-1:0]     RxDcDiffSqQ;
    wire signed [ACC_W-1:0]     ObsDcLastI;
    wire signed [ACC_W-1:0]     ObsDcLastQ;

    integer b;
    integer n;
    integer sign_v;
    integer n_eff;
    integer valid_batches;
    integer pos_batches;
    integer neg_batches;
    integer batch_obs_i_pos;
    integer batch_obs_i_neg;
    integer batch_obs_q_pos;
    integer batch_obs_q_neg;
    integer exp_tx_dc_total_i;
    integer exp_tx_dc_total_q;
    integer exp_obs_dc_total_i;
    integer exp_obs_dc_total_q;
    integer exp_tx_perturb;
    integer exp_tx_perturb_sq;
    integer exp_rx_dc_diff_i;
    integer exp_rx_dc_diff_q;
    integer exp_rx_dc_diff_sq_i;
    integer exp_rx_dc_diff_sq_q;
    integer exp_obs_dc_last_i;
    integer exp_obs_dc_last_q;
    integer plusarg_tmp;

    txlol_hw_engine #(
        .SAMPLE_W(SAMPLE_W),
        .ACC_W(ACC_W),
        .CFG_W(CFG_W)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .start(start),
        .abort(abort),
        .sample_valid(sample_valid),
        .tx_i(tx_i),
        .tx_q(tx_q),
        .obs_i(obs_i),
        .obs_q(obs_q),
        .cfg_num_batches(cfg_num_batches),
        .cfg_samples_per_batch(cfg_samples_per_batch),
        .cfg_guard_samples(cfg_guard_samples),
        .cfg_perturb_amp(cfg_perturb_amp),
        .cfg_start_sign(cfg_start_sign),
        .cfg_sq_mode_sample(cfg_sq_mode_sample),
        .busy(busy),
        .done(done),
        .valid(valid),
        .cfg_error(cfg_error),
        .TxDcTotalI(TxDcTotalI),
        .TxDcTotalQ(TxDcTotalQ),
        .ObsDcTotalI(ObsDcTotalI),
        .ObsDcTotalQ(ObsDcTotalQ),
        .TxPerturb(TxPerturb),
        .TxPerturbSq(TxPerturbSq),
        .RxDcDiffI(RxDcDiffI),
        .RxDcDiffQ(RxDcDiffQ),
        .RxDcDiffSqI(RxDcDiffSqI),
        .RxDcDiffSqQ(RxDcDiffSqQ),
        .ObsDcLastI(ObsDcLastI),
        .ObsDcLastQ(ObsDcLastQ)
    );

    always #5 clk = ~clk;

    initial begin
        clk = 1'b0;
        rst_n = 1'b0;
        start = 1'b0;
        abort = 1'b0;
        sample_valid = 1'b0;
        tx_i = '0;
        tx_q = '0;
        obs_i = '0;
        obs_q = '0;
        cfg_num_batches = 16'd10;
        cfg_samples_per_batch = 16'd32;
        cfg_guard_samples = 16'd0;
        cfg_perturb_amp = 16'd8;
        cfg_start_sign = 1'b1;
        cfg_sq_mode_sample = 1'b0;

        if ($value$plusargs("NUM_BATCHES=%d", plusarg_tmp)) begin
            cfg_num_batches = plusarg_tmp[CFG_W-1:0];
        end
        if ($value$plusargs("SAMPLES_PER_BATCH=%d", plusarg_tmp)) begin
            cfg_samples_per_batch = plusarg_tmp[CFG_W-1:0];
        end
        if ($value$plusargs("GUARD_SAMPLES=%d", plusarg_tmp)) begin
            cfg_guard_samples = plusarg_tmp[CFG_W-1:0];
        end
        if ($value$plusargs("PERTURB_AMP=%d", plusarg_tmp)) begin
            cfg_perturb_amp = plusarg_tmp[CFG_W-1:0];
        end
        if ($value$plusargs("START_SIGN=%d", plusarg_tmp)) begin
            cfg_start_sign = plusarg_tmp[0];
        end
        if ($value$plusargs("SQ_MODE_SAMPLE=%d", plusarg_tmp)) begin
            cfg_sq_mode_sample = plusarg_tmp[0];
        end

        repeat (4) @(posedge clk);
        rst_n = 1'b1;
        @(posedge clk);
        start = 1'b1;
        @(posedge clk);
        start = 1'b0;

        for (b = 0; b < cfg_num_batches; b = b + 1) begin
            sign_v = (b[0] == 0) ? 1 : -1;
            for (n = 0; n < cfg_samples_per_batch; n = n + 1) begin
                @(posedge clk);
                sample_valid <= 1'b1;
                tx_i <= 16'sd100;
                tx_q <= -16'sd20;
                obs_i <= 16'sd300 + (sign_v * $signed(cfg_perturb_amp));
                obs_q <= -16'sd120 + ((sign_v * $signed(cfg_perturb_amp)) >>> 1);
            end
        end

        @(posedge clk);
        sample_valid <= 1'b0;
        tx_i <= '0;
        tx_q <= '0;
        obs_i <= '0;
        obs_q <= '0;

        wait(done);
        @(posedge clk);

        n_eff = cfg_samples_per_batch - (cfg_guard_samples * 2);
        valid_batches = cfg_num_batches - 2;
        pos_batches = valid_batches / 2;
        neg_batches = valid_batches / 2;

        batch_obs_i_pos = (300 + cfg_perturb_amp) * n_eff;
        batch_obs_i_neg = (300 - cfg_perturb_amp) * n_eff;
        batch_obs_q_pos = (-120 + (cfg_perturb_amp >>> 1)) * n_eff;
        batch_obs_q_neg = (-120 - (cfg_perturb_amp >>> 1)) * n_eff;

        exp_tx_dc_total_i = cfg_num_batches * cfg_samples_per_batch * 100;
        exp_tx_dc_total_q = cfg_num_batches * cfg_samples_per_batch * (-20);
        exp_obs_dc_total_i = cfg_num_batches * cfg_samples_per_batch * 300;
        exp_obs_dc_total_q = cfg_num_batches * cfg_samples_per_batch * (-120);
        exp_tx_perturb = valid_batches * n_eff * cfg_perturb_amp;
        exp_tx_perturb_sq = valid_batches * n_eff * cfg_perturb_amp * cfg_perturb_amp;
        exp_rx_dc_diff_i = valid_batches * n_eff * cfg_perturb_amp;
        exp_rx_dc_diff_q = valid_batches * n_eff * (cfg_perturb_amp >>> 1);
        exp_rx_dc_diff_sq_i = pos_batches * batch_obs_i_pos * batch_obs_i_pos +
                      neg_batches * batch_obs_i_neg * batch_obs_i_neg;
        exp_rx_dc_diff_sq_q = pos_batches * batch_obs_q_pos * batch_obs_q_pos +
                      neg_batches * batch_obs_q_neg * batch_obs_q_neg;
        exp_obs_dc_last_i = 300 - cfg_perturb_amp;
        exp_obs_dc_last_q = -120 - (cfg_perturb_amp >>> 1);

        $display("done=%0d valid=%0d cfg_error=%0d", done, valid, cfg_error);
        $display("TxDcTotalI=%0d TxDcTotalQ=%0d", TxDcTotalI, TxDcTotalQ);
        $display("ObsDcTotalI=%0d ObsDcTotalQ=%0d", ObsDcTotalI, ObsDcTotalQ);
        $display("TxPerturb=%0d TxPerturbSq=%0d", TxPerturb, TxPerturbSq);
        $display("RxDcDiffI=%0d RxDcDiffQ=%0d", RxDcDiffI, RxDcDiffQ);
        $display("RxDcDiffSqI=%0d RxDcDiffSqQ=%0d", RxDcDiffSqI, RxDcDiffSqQ);
        $display("ObsDcLastI=%0d ObsDcLastQ=%0d", ObsDcLastI, ObsDcLastQ);
        $display("RTL_RESULT TxDcTotalI=%0d TxDcTotalQ=%0d ObsDcTotalI=%0d ObsDcTotalQ=%0d TxPerturb=%0d TxPerturbSq=%0d RxDcDiffI=%0d RxDcDiffQ=%0d RxDcDiffSqI=%0d RxDcDiffSqQ=%0d ObsDcLastI=%0d ObsDcLastQ=%0d",
             TxDcTotalI, TxDcTotalQ, ObsDcTotalI, ObsDcTotalQ,
             TxPerturb, TxPerturbSq, RxDcDiffI, RxDcDiffQ,
             RxDcDiffSqI, RxDcDiffSqQ, ObsDcLastI, ObsDcLastQ);

        if (done !== 1'b1 || valid !== 1'b1 || cfg_error !== 1'b0) begin
            $fatal(1, "Handshake/status check failed.");
        end

        if (TxDcTotalI !== exp_tx_dc_total_i) begin
            $fatal(1, "TxDcTotalI mismatch. got=%0d exp=%0d", TxDcTotalI, exp_tx_dc_total_i);
        end
        if (TxDcTotalQ !== exp_tx_dc_total_q) begin
            $fatal(1, "TxDcTotalQ mismatch. got=%0d exp=%0d", TxDcTotalQ, exp_tx_dc_total_q);
        end
        if (ObsDcTotalI !== exp_obs_dc_total_i) begin
            $fatal(1, "ObsDcTotalI mismatch. got=%0d exp=%0d", ObsDcTotalI, exp_obs_dc_total_i);
        end
        if (ObsDcTotalQ !== exp_obs_dc_total_q) begin
            $fatal(1, "ObsDcTotalQ mismatch. got=%0d exp=%0d", ObsDcTotalQ, exp_obs_dc_total_q);
        end
        if (TxPerturb !== exp_tx_perturb) begin
            $fatal(1, "TxPerturb mismatch. got=%0d exp=%0d", TxPerturb, exp_tx_perturb);
        end
        if (TxPerturbSq !== exp_tx_perturb_sq) begin
            $fatal(1, "TxPerturbSq mismatch. got=%0d exp=%0d", TxPerturbSq, exp_tx_perturb_sq);
        end
        if (RxDcDiffI !== exp_rx_dc_diff_i) begin
            $fatal(1, "RxDcDiffI mismatch. got=%0d exp=%0d", RxDcDiffI, exp_rx_dc_diff_i);
        end
        if (RxDcDiffQ !== exp_rx_dc_diff_q) begin
            $fatal(1, "RxDcDiffQ mismatch. got=%0d exp=%0d", RxDcDiffQ, exp_rx_dc_diff_q);
        end
        if (RxDcDiffSqI !== exp_rx_dc_diff_sq_i) begin
            $fatal(1, "RxDcDiffSqI mismatch. got=%0d exp=%0d", RxDcDiffSqI, exp_rx_dc_diff_sq_i);
        end
        if (RxDcDiffSqQ !== exp_rx_dc_diff_sq_q) begin
            $fatal(1, "RxDcDiffSqQ mismatch. got=%0d exp=%0d", RxDcDiffSqQ, exp_rx_dc_diff_sq_q);
        end
        if (ObsDcLastI !== exp_obs_dc_last_i) begin
            $fatal(1, "ObsDcLastI mismatch. got=%0d exp=%0d", ObsDcLastI, exp_obs_dc_last_i);
        end
        if (ObsDcLastQ !== exp_obs_dc_last_q) begin
            $fatal(1, "ObsDcLastQ mismatch. got=%0d exp=%0d", ObsDcLastQ, exp_obs_dc_last_q);
        end

        $display("PASS: analytical checks completed.");

        repeat (4) @(posedge clk);
        $finish;
    end

endmodule