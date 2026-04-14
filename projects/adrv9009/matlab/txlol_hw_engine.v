module txlol_hw_engine #(
    parameter integer SAMPLE_W = 16,
    parameter integer ACC_W    = 64,
    parameter integer CFG_W    = 16
)(
    input  wire                         clk,
    input  wire                         rst_n,
    input  wire                         start,
    input  wire                         abort,
    input  wire                         sample_valid,
    input  wire signed [SAMPLE_W-1:0]   tx_i,
    input  wire signed [SAMPLE_W-1:0]   tx_q,
    input  wire signed [SAMPLE_W-1:0]   obs_i,
    input  wire signed [SAMPLE_W-1:0]   obs_q,
    input  wire        [CFG_W-1:0]      cfg_num_batches,
    input  wire        [CFG_W-1:0]      cfg_samples_per_batch,
    input  wire        [CFG_W-1:0]      cfg_guard_samples,
    input  wire        [CFG_W-1:0]      cfg_perturb_amp,
    input  wire                         cfg_start_sign,
    input  wire                         cfg_sq_mode_sample,
    output reg                          busy,
    output reg                          done,
    output reg                          valid,
    output reg                          cfg_error,
    output reg signed [ACC_W-1:0]       TxDcTotalI,
    output reg signed [ACC_W-1:0]       TxDcTotalQ,
    output reg signed [ACC_W-1:0]       ObsDcTotalI,
    output reg signed [ACC_W-1:0]       ObsDcTotalQ,
    output reg signed [ACC_W-1:0]       TxPerturb,
    output reg signed [ACC_W-1:0]       TxPerturbSq,
    output reg signed [ACC_W-1:0]       RxDcDiffI,
    output reg signed [ACC_W-1:0]       RxDcDiffQ,
    output reg signed [ACC_W-1:0]       RxDcDiffSqI,
    output reg signed [ACC_W-1:0]       RxDcDiffSqQ,
    output reg signed [ACC_W-1:0]       ObsDcLastI,
    output reg signed [ACC_W-1:0]       ObsDcLastQ
);

    localparam [1:0] ST_IDLE     = 2'd0;
    localparam [1:0] ST_RUN      = 2'd1;
    localparam [1:0] ST_FINALIZE = 2'd2;

    reg [1:0] state;

    reg [CFG_W-1:0] num_batches_l;
    reg [CFG_W-1:0] samples_per_batch_l;
    reg [CFG_W-1:0] guard_samples_l;
    reg [CFG_W-1:0] perturb_amp_l;
    reg             start_sign_l;
    reg             sq_mode_sample_l;

    reg [CFG_W-1:0] batch_idx;
    reg [CFG_W-1:0] sample_idx;
    reg             batch_sign;

    reg signed [ACC_W-1:0] tx_dc_total_i_acc;
    reg signed [ACC_W-1:0] tx_dc_total_q_acc;
    reg signed [ACC_W-1:0] obs_dc_total_i_acc;
    reg signed [ACC_W-1:0] obs_dc_total_q_acc;
    reg signed [ACC_W-1:0] tx_perturb_acc;
    reg signed [ACC_W-1:0] tx_perturb_sq_acc;
    reg signed [ACC_W-1:0] rx_dc_diff_i_acc;
    reg signed [ACC_W-1:0] rx_dc_diff_q_acc;
    reg signed [ACC_W-1:0] rx_dc_diff_sq_i_acc;
    reg signed [ACC_W-1:0] rx_dc_diff_sq_q_acc;
    reg signed [ACC_W-1:0] obs_dc_last_i_acc;
    reg signed [ACC_W-1:0] obs_dc_last_q_acc;

    reg signed [ACC_W-1:0] batch_obs_i_acc;
    reg signed [ACC_W-1:0] batch_obs_q_acc;
    reg signed [ACC_W-1:0] batch_obs_sq_i_acc;
    reg signed [ACC_W-1:0] batch_obs_sq_q_acc;

    wire signed [ACC_W-1:0] tx_i_ext;
    wire signed [ACC_W-1:0] tx_q_ext;
    wire signed [ACC_W-1:0] obs_i_ext;
    wire signed [ACC_W-1:0] obs_q_ext;
    wire signed [ACC_W-1:0] perturb_amp_ext;
    wire signed [(2*SAMPLE_W)-1:0] obs_i_sq_full;
    wire signed [(2*SAMPLE_W)-1:0] obs_q_sq_full;
    wire signed [(2*CFG_W)-1:0]    perturb_sq_full;

    wire signed [ACC_W-1:0] obs_i_sq_ext;
    wire signed [ACC_W-1:0] obs_q_sq_ext;
    wire signed [ACC_W-1:0] perturb_sq_ext;

    wire                     in_guard_low;
    wire                     in_guard_high;
    wire                     sample_in_valid_region;
    wire                     batch_is_valid;
    wire                     last_sample_in_batch;
    wire                     last_batch_in_capture;
    wire                     cfg_ok;
    wire signed [ACC_W-1:0] batch_obs_i_next;
    wire signed [ACC_W-1:0] batch_obs_q_next;
    wire signed [ACC_W-1:0] batch_obs_sq_i_next;
    wire signed [ACC_W-1:0] batch_obs_sq_q_next;

    assign tx_i_ext       = {{(ACC_W-SAMPLE_W){tx_i[SAMPLE_W-1]}}, tx_i};
    assign tx_q_ext       = {{(ACC_W-SAMPLE_W){tx_q[SAMPLE_W-1]}}, tx_q};
    assign obs_i_ext      = {{(ACC_W-SAMPLE_W){obs_i[SAMPLE_W-1]}}, obs_i};
    assign obs_q_ext      = {{(ACC_W-SAMPLE_W){obs_q[SAMPLE_W-1]}}, obs_q};
    assign perturb_amp_ext = {{(ACC_W-CFG_W){1'b0}}, perturb_amp_l};

    assign obs_i_sq_full  = obs_i * obs_i;
    assign obs_q_sq_full  = obs_q * obs_q;
    assign perturb_sq_full = perturb_amp_l * perturb_amp_l;

    assign obs_i_sq_ext   = {{(ACC_W-(2*SAMPLE_W)){1'b0}}, obs_i_sq_full};
    assign obs_q_sq_ext   = {{(ACC_W-(2*SAMPLE_W)){1'b0}}, obs_q_sq_full};
    assign perturb_sq_ext = {{(ACC_W-(2*CFG_W)){1'b0}}, perturb_sq_full};

    assign in_guard_low          = (sample_idx < guard_samples_l);
    assign in_guard_high         = (sample_idx >= (samples_per_batch_l - guard_samples_l));
    assign sample_in_valid_region = ~in_guard_low & ~in_guard_high;
    assign batch_is_valid        = (batch_idx >= {{(CFG_W-1){1'b0}},1'b1}) &&
                                   (batch_idx < (num_batches_l - {{(CFG_W-1){1'b0}},1'b1}));
    assign last_sample_in_batch  = (sample_idx == (samples_per_batch_l - {{(CFG_W-1){1'b0}},1'b1}));
    assign last_batch_in_capture = (batch_idx == (num_batches_l - {{(CFG_W-1){1'b0}},1'b1}));
    assign cfg_ok                = (cfg_num_batches >= 16'd4) &&
                                   (cfg_num_batches[0] == 1'b0) &&
                                   (cfg_samples_per_batch > (cfg_guard_samples << 1));
    assign batch_obs_i_next      = batch_obs_i_acc + (sample_in_valid_region ? obs_i_ext : {ACC_W{1'b0}});
    assign batch_obs_q_next      = batch_obs_q_acc + (sample_in_valid_region ? obs_q_ext : {ACC_W{1'b0}});
    assign batch_obs_sq_i_next   = batch_obs_sq_i_acc + (sample_in_valid_region ? obs_i_sq_ext : {ACC_W{1'b0}});
    assign batch_obs_sq_q_next   = batch_obs_sq_q_acc + (sample_in_valid_region ? obs_q_sq_ext : {ACC_W{1'b0}});

    task clear_active_accumulators;
    begin
        tx_dc_total_i_acc  = {ACC_W{1'b0}};
        tx_dc_total_q_acc  = {ACC_W{1'b0}};
        obs_dc_total_i_acc = {ACC_W{1'b0}};
        obs_dc_total_q_acc = {ACC_W{1'b0}};
        tx_perturb_acc     = {ACC_W{1'b0}};
        tx_perturb_sq_acc  = {ACC_W{1'b0}};
        rx_dc_diff_i_acc   = {ACC_W{1'b0}};
        rx_dc_diff_q_acc   = {ACC_W{1'b0}};
        rx_dc_diff_sq_i_acc = {ACC_W{1'b0}};
        rx_dc_diff_sq_q_acc = {ACC_W{1'b0}};
        obs_dc_last_i_acc  = {ACC_W{1'b0}};
        obs_dc_last_q_acc  = {ACC_W{1'b0}};
        batch_obs_i_acc    = {ACC_W{1'b0}};
        batch_obs_q_acc    = {ACC_W{1'b0}};
        batch_obs_sq_i_acc = {ACC_W{1'b0}};
        batch_obs_sq_q_acc = {ACC_W{1'b0}};
    end
    endtask

    task clear_batch_accumulators;
    begin
        batch_obs_i_acc    = {ACC_W{1'b0}};
        batch_obs_q_acc    = {ACC_W{1'b0}};
        batch_obs_sq_i_acc = {ACC_W{1'b0}};
        batch_obs_sq_q_acc = {ACC_W{1'b0}};
    end
    endtask

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            state      <= ST_IDLE;
            busy       <= 1'b0;
            done       <= 1'b0;
            valid      <= 1'b0;
            cfg_error  <= 1'b0;
            num_batches_l       <= {CFG_W{1'b0}};
            samples_per_batch_l <= {CFG_W{1'b0}};
            guard_samples_l     <= {CFG_W{1'b0}};
            perturb_amp_l       <= {CFG_W{1'b0}};
            start_sign_l        <= 1'b0;
            sq_mode_sample_l    <= 1'b0;
            batch_idx           <= {CFG_W{1'b0}};
            sample_idx          <= {CFG_W{1'b0}};
            batch_sign          <= 1'b0;
            clear_active_accumulators();
            TxDcTotalI <= {ACC_W{1'b0}};
            TxDcTotalQ <= {ACC_W{1'b0}};
            ObsDcTotalI <= {ACC_W{1'b0}};
            ObsDcTotalQ <= {ACC_W{1'b0}};
            TxPerturb <= {ACC_W{1'b0}};
            TxPerturbSq <= {ACC_W{1'b0}};
            RxDcDiffI <= {ACC_W{1'b0}};
            RxDcDiffQ <= {ACC_W{1'b0}};
            RxDcDiffSqI <= {ACC_W{1'b0}};
            RxDcDiffSqQ <= {ACC_W{1'b0}};
            ObsDcLastI <= {ACC_W{1'b0}};
            ObsDcLastQ <= {ACC_W{1'b0}};
        end else begin
            done <= 1'b0;

            case (state)
                ST_IDLE: begin
                    busy <= 1'b0;
                    if (start) begin
                        valid <= 1'b0;
                        cfg_error <= 1'b0;
                        num_batches_l       <= cfg_num_batches;
                        samples_per_batch_l <= cfg_samples_per_batch;
                        guard_samples_l     <= cfg_guard_samples;
                        perturb_amp_l       <= cfg_perturb_amp;
                        start_sign_l        <= cfg_start_sign;
                        sq_mode_sample_l    <= cfg_sq_mode_sample;

                        if (!cfg_ok) begin
                            cfg_error <= 1'b1;
                            valid <= 1'b0;
                        end else begin
                            clear_active_accumulators();
                            batch_idx  <= {CFG_W{1'b0}};
                            sample_idx <= {CFG_W{1'b0}};
                            batch_sign <= cfg_start_sign;
                            busy <= 1'b1;
                            state <= ST_RUN;
                        end
                    end
                end

                ST_RUN: begin
                    if (abort) begin
                        busy  <= 1'b0;
                        valid <= 1'b0;
                        state <= ST_IDLE;
                    end else if (sample_valid) begin
                        tx_dc_total_i_acc  <= tx_dc_total_i_acc + tx_i_ext;
                        tx_dc_total_q_acc  <= tx_dc_total_q_acc + tx_q_ext;
                        obs_dc_total_i_acc <= obs_dc_total_i_acc + obs_i_ext;
                        obs_dc_total_q_acc <= obs_dc_total_q_acc + obs_q_ext;
                        obs_dc_last_i_acc  <= obs_i_ext;
                        obs_dc_last_q_acc  <= obs_q_ext;

                        if (sample_in_valid_region && batch_is_valid) begin
                            tx_perturb_acc    <= tx_perturb_acc + perturb_amp_ext;
                            tx_perturb_sq_acc <= tx_perturb_sq_acc + perturb_sq_ext;
                            if (sq_mode_sample_l) begin
                                rx_dc_diff_sq_i_acc <= rx_dc_diff_sq_i_acc + obs_i_sq_ext;
                                rx_dc_diff_sq_q_acc <= rx_dc_diff_sq_q_acc + obs_q_sq_ext;
                            end
                        end

                        if (last_sample_in_batch) begin
                            if (batch_is_valid) begin
                                if (batch_sign) begin
                                    rx_dc_diff_i_acc <= rx_dc_diff_i_acc + batch_obs_i_next;
                                    rx_dc_diff_q_acc <= rx_dc_diff_q_acc + batch_obs_q_next;
                                end else begin
                                    rx_dc_diff_i_acc <= rx_dc_diff_i_acc - batch_obs_i_next;
                                    rx_dc_diff_q_acc <= rx_dc_diff_q_acc - batch_obs_q_next;
                                end
                                if (!sq_mode_sample_l) begin
                                    rx_dc_diff_sq_i_acc <= rx_dc_diff_sq_i_acc +
                                                           batch_obs_i_next * batch_obs_i_next;
                                    rx_dc_diff_sq_q_acc <= rx_dc_diff_sq_q_acc +
                                                           batch_obs_q_next * batch_obs_q_next;
                                end
                            end

                            if (last_batch_in_capture) begin
                                batch_obs_i_acc    <= {ACC_W{1'b0}};
                                batch_obs_q_acc    <= {ACC_W{1'b0}};
                                batch_obs_sq_i_acc <= {ACC_W{1'b0}};
                                batch_obs_sq_q_acc <= {ACC_W{1'b0}};
                                state <= ST_FINALIZE;
                            end else begin
                                batch_idx  <= batch_idx + {{(CFG_W-1){1'b0}},1'b1};
                                sample_idx <= {CFG_W{1'b0}};
                                batch_sign <= ~batch_sign;
                                batch_obs_i_acc    <= {ACC_W{1'b0}};
                                batch_obs_q_acc    <= {ACC_W{1'b0}};
                                batch_obs_sq_i_acc <= {ACC_W{1'b0}};
                                batch_obs_sq_q_acc <= {ACC_W{1'b0}};
                            end
                        end else begin
                            batch_obs_i_acc    <= batch_obs_i_next;
                            batch_obs_q_acc    <= batch_obs_q_next;
                            batch_obs_sq_i_acc <= batch_obs_sq_i_next;
                            batch_obs_sq_q_acc <= batch_obs_sq_q_next;
                            sample_idx <= sample_idx + {{(CFG_W-1){1'b0}},1'b1};
                        end
                    end
                end

                ST_FINALIZE: begin
                    busy <= 1'b0;
                    done <= 1'b1;
                    valid <= 1'b1;

                    TxDcTotalI <= tx_dc_total_i_acc;
                    TxDcTotalQ <= tx_dc_total_q_acc;
                    ObsDcTotalI <= obs_dc_total_i_acc;
                    ObsDcTotalQ <= obs_dc_total_q_acc;
                    TxPerturb <= tx_perturb_acc;
                    TxPerturbSq <= tx_perturb_sq_acc;
                    RxDcDiffI <= rx_dc_diff_i_acc;
                    RxDcDiffQ <= rx_dc_diff_q_acc;
                    RxDcDiffSqI <= rx_dc_diff_sq_i_acc;
                    RxDcDiffSqQ <= rx_dc_diff_sq_q_acc;
                    ObsDcLastI <= obs_dc_last_i_acc;
                    ObsDcLastQ <= obs_dc_last_q_acc;

                    state <= ST_IDLE;
                end

                default: begin
                    state <= ST_IDLE;
                end
            endcase
        end
    end

endmodule