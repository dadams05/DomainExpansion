#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <math.h>

// Main code
int main(int, char **)
{
    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    // Create window with SDL_Renderer graphics context
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    std::cout << "Display scale: x" << main_scale << std::endl;
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window *window = SDL_CreateWindow("DomainExpansion", (int)(960 * main_scale), (int)(720 * main_scale), window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();
    // style.FontSizeBase = 16.0f;           // Default to 13.0f when unspecified, which allows crisp legacy font for non-4K screens.
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;

    // Setup scaling
    style.ScaleAllSizes(main_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale; // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefaultVector();
    // io.Fonts->AddFontDefaultBitmap();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    // IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(1.00f, 0.55f, 0.00f, 1.00f);

    // Main loop
    bool done = false;

    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (!ImGui::Begin("Example: Custom rendering"))
        {
            ImGui::End();
        }
        else
        {

            // Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of
            // overloaded operators, etc. Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your
            // types and ImVec2/ImVec4. Dear ImGui defines overloaded operators but they are internal to imgui.cpp and not
            // exposed outside (to avoid messing with your types) In this example we are not using the maths operators!

            if (ImGui::BeginTabBar("##TabBar"))
            {
                if (ImGui::BeginTabItem("Canvas"))
                {
                    static ImVector<ImVec2> points;
                    static ImVec2 scrolling(0.0f, 0.0f);
                    static bool opt_enable_grid = true;
                    static bool opt_enable_context_menu = true;
                    static bool adding_line = false;

                    ImGui::Checkbox("Enable grid", &opt_enable_grid);
                    ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
                    ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

                    // Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping region + own scrolling.
                    // Here we demonstrate that this can be replaced by simple offsetting + custom drawing + PushClipRect/PopClipRect() calls.
                    // To use a child window instead we could use, e.g:
                    //      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
                    //      ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
                    //      ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove);
                    //      ImGui::PopStyleColor();
                    //      ImGui::PopStyleVar();
                    //      [...]
                    //      ImGui::EndChild();

                    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
                    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();    // ImDrawList API uses screen coordinates!
                    ImVec2 canvas_sz = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
                    if (canvas_sz.x < 50.0f)
                        canvas_sz.x = 50.0f;
                    if (canvas_sz.y < 50.0f)
                        canvas_sz.y = 50.0f;
                    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

                    // Draw border and background color
                    ImGuiIO &io = ImGui::GetIO();
                    ImDrawList *draw_list = ImGui::GetWindowDrawList();
                    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
                    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

                    // This will catch our interactions
                    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
                    const bool is_hovered = ImGui::IsItemHovered();                            // Hovered
                    const bool is_active = ImGui::IsItemActive();                              // Held
                    const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
                    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

                    // Add first and second point
                    if (is_hovered && !adding_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        points.push_back(mouse_pos_in_canvas);
                        points.push_back(mouse_pos_in_canvas);
                        adding_line = true;
                    }
                    if (adding_line)
                    {
                        points.back() = mouse_pos_in_canvas;
                        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                            adding_line = false;
                    }

                    // Pan (we use a zero mouse threshold when there's no context menu)
                    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
                    const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
                    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
                    {
                        scrolling.x += io.MouseDelta.x;
                        scrolling.y += io.MouseDelta.y;
                    }

                    // Context menu (under default mouse threshold)
                    ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
                    if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
                        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
                    if (ImGui::BeginPopup("context"))
                    {
                        if (adding_line)
                            points.resize(points.size() - 2);
                        adding_line = false;
                        if (ImGui::MenuItem("Remove one", NULL, false, points.Size > 0))
                        {
                            points.resize(points.size() - 2);
                        }
                        if (ImGui::MenuItem("Remove all", NULL, false, points.Size > 0))
                        {
                            points.clear();
                        }
                        ImGui::EndPopup();
                    }

                    // Draw grid + all lines in the canvas
                    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
                    if (opt_enable_grid)
                    {
                        const float GRID_STEP = 64.0f;
                        for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                            draw_list->AddLineV(canvas_p0.x + x, canvas_p0.y, canvas_p1.y, IM_COL32(200, 200, 200, 40));
                        for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                            draw_list->AddLineH(canvas_p0.x, canvas_p1.x, canvas_p0.y + y, IM_COL32(200, 200, 200, 40));
                    }
                    for (int n = 0; n < points.Size; n += 2)
                        draw_list->AddLine(ImVec2(origin.x + points[n].x, origin.y + points[n].y), ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
                    draw_list->PopClipRect();

                    ImGui::EndTabItem();

                    std::cout << "x: " << scrolling.x << " y: " << scrolling.y << std::endl;
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }
        // Rendering
        ImGui::Render();
        SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}












// // Dear ImGui: standalone example application for SDL3 + SDL_Renderer
// // (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// // Learn about Dear ImGui:
// // - FAQ                  https://dearimgui.com/faq
// // - Getting Started      https://dearimgui.com/getting-started
// // - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// // - Introduction, links and more at the top of imgui.cpp

// // Important to understand: SDL_Renderer is an _optional_ component of SDL3.
// // For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

// #include "imgui.h"
// #include "imgui_impl_sdl3.h"
// #include "imgui_impl_sdlrenderer3.h"
// #include <stdio.h>
// #include <SDL3/SDL.h>

// #ifdef __EMSCRIPTEN__
// #include "../libs/emscripten/emscripten_mainloop_stub.h"
// #endif

// // Main code
// int main(int, char**)
// {
//     // Setup SDL
//     // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
//     if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
//     {
//         printf("Error: SDL_Init(): %s\n", SDL_GetError());
//         return 1;
//     }

//     // Create window with SDL_Renderer graphics context
//     float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
//     SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
//     SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
//     if (window == nullptr)
//     {
//         printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
//         return 1;
//     }
//     SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
//     SDL_SetRenderVSync(renderer, 1);
//     if (renderer == nullptr)
//     {
//         SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
//         return 1;
//     }
//     SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
//     SDL_ShowWindow(window);

//     // Setup Dear ImGui context
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
//     ImGuiIO& io = ImGui::GetIO(); (void)io;
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

//     // Setup Dear ImGui style
//     ImGuiStyle& style = ImGui::GetStyle();
//     ImGui::StyleColorsDark();
//     //ImGui::StyleColorsLight();
//     //style.FontSizeBase = 16.0f;           // Default to 13.0f when unspecified, which allows crisp legacy font for non-4K screens.
//     style.FrameRounding = 3.0f;
//     style.GrabRounding = 3.0f;

//     // Setup scaling
//     style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
//     style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

//     // Setup Platform/Renderer backends
//     ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
//     ImGui_ImplSDLRenderer3_Init(renderer);

//     // Load Fonts
//     // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
//     //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a threshold.
//     // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
//     // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
//     // - Read 'docs/FONTS.md' for more instructions and details.
//     // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
//     // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
//     //io.Fonts->AddFontDefaultVector();
//     //io.Fonts->AddFontDefaultBitmap();
//     //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
//     //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
//     //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
//     //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
//     //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
//     //IM_ASSERT(font != nullptr);

//     // Our state
//     bool show_demo_window = true;
//     bool show_another_window = false;
//     ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

//     // Main loop
//     bool done = false;
// #ifdef __EMSCRIPTEN__
//     // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
//     // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
//     io.IniFilename = nullptr;
//     EMSCRIPTEN_MAINLOOP_BEGIN
// #else
//     while (!done)
// #endif
//     {
//         // Poll and handle events (inputs, window resize, etc.)
//         // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
//         // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
//         // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
//         // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
//         // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
//         SDL_Event event;
//         while (SDL_PollEvent(&event))
//         {
//             ImGui_ImplSDL3_ProcessEvent(&event);
//             if (event.type == SDL_EVENT_QUIT)
//                 done = true;
//             if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
//                 done = true;
//         }

//         // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
//         if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
//         {
//             SDL_Delay(10);
//             continue;
//         }

//         // Start the Dear ImGui frame
//         ImGui_ImplSDLRenderer3_NewFrame();
//         ImGui_ImplSDL3_NewFrame();
//         ImGui::NewFrame();

//         // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//         if (show_demo_window)
//             ImGui::ShowDemoWindow(&show_demo_window);

//         // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
//         {
//             static float f = 0.0f;
//             static int counter = 0;

//             ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

//             ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//             ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//             ImGui::Checkbox("Another Window", &show_another_window);

//             ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//             ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

//             if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//                 counter++;
//             ImGui::SameLine();
//             ImGui::Text("counter = %d", counter);

//             ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
//             ImGui::End();
//         }

//         // 3. Show another simple window.
//         if (show_another_window)
//         {
//             ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//             ImGui::Text("Hello from another window!");
//             if (ImGui::Button("Close Me"))
//                 show_another_window = false;
//             ImGui::End();
//         }

//         // Rendering
//         ImGui::Render();
//         SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
//         SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//         SDL_RenderClear(renderer);
//         ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
//         SDL_RenderPresent(renderer);
//     }
// #ifdef __EMSCRIPTEN__
//     EMSCRIPTEN_MAINLOOP_END;
// #endif

//     // Cleanup
//     // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
//     ImGui_ImplSDLRenderer3_Shutdown();
//     ImGui_ImplSDL3_Shutdown();
//     ImGui::DestroyContext();

//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     SDL_Quit();

//     return 0;
// }
