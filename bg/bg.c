#include <adx/adx.h>
#include <dc/sound/sound.h>
#include <kos.h>
#include <adx/snddrv.h> 
#include <stdint.h>

// Use default init settings
KOS_INIT_FLAGS(INIT_DEFAULT);

int main(int arc, char* argv[])
{
    /*
        Setup custom image renderng settings
    */
    pvr_init_params_t params =
    {
        .opb_sizes = { PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0, PVR_BINSIZE_0, PVR_BINSIZE_0 },
        .vertex_buf_size = 512 * 1024,
        .fsaa_enabled = 0,
        .dma_enabled = 0,
        .autosort_disabled = 0,
        .opb_overflow_count = 0,
        .vbuf_doublebuf_disabled = 0
    };
    pvr_init(&params);

    // Initialize controller in slot 1 and see if it exists
    maple_device_t* controller = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if(!controller)
    {
        arch_menu();
        return -1;
    }

    // Attempt to load music file as compressed ADX (natively supported)
    if(adx_dec("/rd/bgm.adx", 1) < 1)
    {
        arch_menu();
        return -1;
    }

    // Wait for the sound driver to start up
    while(snddrv.drv_status == SNDDRV_STATUS_NULL)
    {
        thd_pass();
    }

    // Establish variables for the background image
    const float bgSizePixel = 1024.0f;
    const size_t bgSizeByte = (size_t)(bgSizePixel * bgSizePixel * 2);
    const float bgImageWidth = 800.0f;
    const float bgImageHeight = 600.0f;
    const float bgRenderImageWidth = 640.0f;
    const float bgRenderImageHeight = 480.0f;

    // Allocate memory
    pvr_ptr_t bgVRAM = pvr_mem_malloc(bgSizeByte);
    // Open file for background image
    file_t file = fs_open("/rd/bg.dt", O_RDONLY);
    // If it doesn't exist, exit
    if(file == FILEHND_INVALID)
    {
        if(bgVRAM)
        {
            pvr_mem_free(bgVRAM);
        }
        arch_menu();
        return -1;
    }
    // If it does, read it into RAM and then close the file
    fs_read(file, bgVRAM, bgSizeByte);
    fs_close(file);

    // Set up the bakcground texture
    pvr_poly_cxt_t bgContext;
    pvr_poly_hdr_t bgHeader;
    // Standard 16bit RGB format, non-twiddled, bilinear filter when scaled down
    pvr_poly_cxt_txr(&bgContext, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED, (int)bgSizePixel, (int)bgSizePixel, bgVRAM, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&bgHeader, &bgContext);

    /*
        Fancy math that will make sure 800x600 at the top left corner of a 1024x1024 container
        will render at perfectly within the 640x480 display
    */
    const float uOffset = 16.0f;
    const float vOffset = 0.5f;
    const float uMin = uOffset / bgSizePixel;
    const float vMin = vOffset / bgSizePixel;
    const float uMax = (bgImageWidth + uOffset) / bgSizePixel;
    const float vMax = (bgImageHeight - vOffset) / bgSizePixel;
    // Setup base color and transparency
    const uint32_t color = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);

    // Define the 4 vertices of the quad the texture will be rendered on
    pvr_vertex_t bgQuad[4] =
    {
        // Vertex, x, y, z, u, v, primary color, offset color
        // Top left
        { PVR_CMD_VERTEX, 0.0f, 0.0f, 1.0f, uMin, vMin, color, 0 },
        // Top right
        { PVR_CMD_VERTEX, bgRenderImageWidth, 0.0f, 1.0f, uMax, vMin, color, 0 },
        // Bottom left
        { PVR_CMD_VERTEX, 0.0f, bgRenderImageHeight, 1.0f, uMin, vMax, color, 0 },
        // Bottom right
        { PVR_CMD_VERTEX_EOL, bgRenderImageWidth, bgRenderImageHeight, 1.0f, uMax, vMax, color, 0 }
    };

    // Game loop
    int running = 1;
    while(running)
    {
        // If the sound driver fails or something break
        if(snddrv.drv_status == SNDDRV_STATUS_NULL)
        {
            running = 0;
        }

        // Check if the controller exists currently and if the A button is pressed to quit
        cont_state_t* state = (cont_state_t*)maple_dev_status(controller);
        if(state && (state->buttons & CONT_A))
        {
            running = 0;
        }

        // Start image rendering
        pvr_wait_ready();
        pvr_scene_begin();
        pvr_list_begin(PVR_LIST_OP_POLY);

        // Render the quad the background texture is in
        pvr_prim(&bgHeader, sizeof(bgHeader));
        for(int i = 0; i < 4; i++)
        {
            pvr_prim(&bgQuad[i], sizeof(pvr_vertex_t));
        }

        // Finish image rendering
        pvr_list_finish();
        pvr_scene_finish();
    }

    // Cleanup
    if(bgVRAM)
    {
        pvr_mem_free(bgVRAM);
    }

    // Return to system menu
    arch_menu();

    return 0;
}
