#include <iostream>
#include <SFML/Graphics.hpp>
#include <complex>

// Plots the Mandelbrot set for the specified amount of iterations
// Left-click on any point to center it to the window
// Use the mouse wheel to zoom in and out

/*Tip: if you want to zoom in somewhere near the edge of the plot, zoom in in the middle first and then use
left-click to move the view to the edge (this way the view does not get clipped by the edge of the rendered screen)*/

std::complex<double> f(std::complex<double> z, std::complex<double> c)
{
    return std::pow(z, 2) + c;
}


sf::VertexArray create_points(int width, int height, int unit, int max_iterations, int Color[3], double x0, double y0)
{
    // Returns array of vertices colored according to escape time

    double R = 2; // escape radius

    sf::VertexArray vertices(sf::Points, width*height);
    sf::Vertex vertex;

    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            double x_t = x + (x0 - width/2);
            double y_t = y + (y0 - height/2);
            double x_num = 1.0*x_t/unit - 1.0*width/(2*unit);
            double y_num = -1.0*y_t/unit + 1.0*height/(2*unit);

            std::complex<double> z(0, 0);
            std::complex<double> c(x_num, y_num);

            int color_index = 0;
            for (int n=0; n<=max_iterations; n++)
            {
                color_index = n;
                std::complex<double> Func = f(z, c);
                z = Func;
                double dist_sqr = z.real()*z.real() + z.imag()*z.imag();

                if (dist_sqr > R*R)
                    break;
            }

            double R = (Color[0] + color_index*Color[0]/max_iterations) % 255;
            double G = (Color[1] + color_index*Color[1]/max_iterations) % 255;
            double B = (Color[2] + color_index*Color[2]/max_iterations) % 255;

            vertex.position = sf::Vector2f(x, y);
            vertex.color = sf::Color(R, G, B);
            vertices.append(vertex);
        }
    }
    return vertices;
}

int main()
{

    int height = 700;
    int width = 900;
    int unit = 400; // amount of pixels that correspond to a unitary distance in the complex plane
    int zoom = 200; // how fast is the zoom

    int x0 = width/2; // the plot is shifted to be centered at (width-x0, height-y0) point
    int y0 = height/2;

    int max_iterations = 42;
    int Color[3] = {0, 255, 0};

    sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot set");
    sf::VertexArray vertices = create_points(width, height, unit, max_iterations, Color, x0, y0);


    // MAIN LOOP
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Left-click centering
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    x0 = event.mouseButton.x + (x0 - width/2);
                    y0 = event.mouseButton.y + (y0 - height/2);

                    std::cout << "Moving the view..." << std::endl;
                    vertices = create_points(width, height, unit, max_iterations, Color, x0, y0);
                    std::cout << "Done" << std::endl;
                }
            }

            // Mouse-wheel zoom
            if (event.type == sf::Event::MouseWheelMoved)
            {
                double unit_new;
                double unit_ratio;

                int delta = event.mouseWheel.delta;
                if (delta > 0)
                    unit_new = unit + zoom*(1 + unit/1000); // speed up the zooming as unit increases
                if (delta < 0)
                    unit_new = unit - zoom*(1 + unit/1000);
                if (unit_new - zoom < zoom)
                    unit_new = zoom;

                // When the plot is rescaled using the new unit, we need to account for the apparent shift of the centered point
                unit_ratio = unit_new/unit;
                x0 = (x0 - width/2)*unit_ratio + width/2;
                y0 = (y0 - height/2)*unit_ratio + height/2;

                std::cout << "Zooming..." << std::endl;
                vertices = create_points(width, height, unit_new, max_iterations, Color, x0, y0);
                std::cout << "Done" << std::endl;

                unit = unit_new;
            }
        }

        window.clear(sf::Color::White);
        window.draw(vertices);

        window.display();
        window.setFramerateLimit(30);
    }
    return 0;
}
