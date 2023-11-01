#include <glad/glad.h>

// GLAD must be included first. clang-format sorts includes, hence this comment
// here.

#include <GLFW/glfw3.h>
#include "window.hpp"
#include "movable_rect.hpp"


static GLFWwindow* window = nullptr;        // Window instance

// List of movable rectangles. This will be important in saving config.
static List<UI_Movable_Rect_S*> movable_rectangles;
static UI_Movable_Rect_S* currently_moved_rectangle = nullptr;  // Currently dragged rectangle.
static f32 last_x, last_y;    // Last mouse x and y position



/**
 *  Template function for interpreting mouse input.
 */
static void mouse_button_callbacks(GLFWwindow* win, int button, int action, int mods)
{
    (void) mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            double x, y;
            glfwGetCursorPos(win, &x, &y);
            f32 xpos = static_cast<f32>(x);
            f32 ypos = static_cast<f32>(y);
            // Check if the mouse click is within any rectangle
            for(auto & mr : movable_rectangles)
            {
                if (UI_Movable_Rect_Is_Under_Mouse(mr, xpos, ypos))
                {
                    currently_moved_rectangle = mr;
                    last_x = xpos;
                    last_y = ypos;
                }
            }

        } else if (action == GLFW_RELEASE)
        {
            currently_moved_rectangle = nullptr;
        }
    }
}

/**
 * Template function for interpreting cursor position.
 */
static void cursor_position_callbacks(GLFWwindow* win, double xpos, double ypos)
{
    (void) win;
    if (currently_moved_rectangle != nullptr)
    {
        f32 x =  static_cast<f32>(xpos);
        f32 y =  static_cast<f32>(ypos);
        // Calculate the new position of the rectangle based on mouse movement
        f32 x_offset =  x - last_x;
        f32 y_offset = y - last_y;
        UI_Movable_Rect_Move(currently_moved_rectangle, x_offset, y_offset);
        last_x = x;
        last_y = y;
    }
}

// It's example of Movable Rectangle's render function.
// Any other function can be passed to the constructor as long as it matches the pattern:
// return -> type void
// Single parameter -> type Rect_f32
static void render(anton::Rect_f32 position)
{
    GLfloat vertices[] = {
            position.left, position.bottom,
            position.right, position.bottom,
            position.right, position.top,
            position.left, position.top
    };

    // Create and configure the VAO (Vertex Array Object) and VBO (Vertex Buffer Object)
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Specify the layout of the vertex data
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Draw the rectangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

u8 UI_Window_Init()
{
    if(window != nullptr)
    {
        return 1;
    }
    // Initialize GLFW
    if(!glfwInit())
    {
        return 1;
    }
    // Create a GLFW window
    window = glfwCreateWindow(1200, 800, "Nebula", nullptr, nullptr);

    if(!window)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    bool const glad_initialised = gladLoadGL();
    if(!glad_initialised)
    {
        return 1;
    }

    // Add mouse callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callbacks);
    glfwSetCursorPosCallback(window, cursor_position_callbacks);

    movable_rectangles.emplace_front(UI_Movable_Rect_Init(&render));
    return 0;
}

void UI_Window_Start_Main_Loop()
{
    if(window == nullptr)
        return;
    // Main loop
    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        for(auto & mr : movable_rectangles)
        {
            UI_Movable_Rect_Render(mr);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void UI_Window_Terminate()
{
    if(window == nullptr)
        return;
    glfwTerminate();
    movable_rectangles.clear();
}