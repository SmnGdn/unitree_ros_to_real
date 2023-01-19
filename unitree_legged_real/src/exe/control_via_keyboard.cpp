#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <termios.h>
#include <ctime>
#include<unistd.h>
#include<signal.h>
#include <string>
#include <time.h>

// char getch() {
//         char buf = 0;
//         struct termios old = {0};
//         if (tcgetattr(0, &old) < 0)
//                 perror("tcsetattr()");
//         old.c_lflag &= ~ICANON;
//         old.c_lflag &= ~ECHO;
//         old.c_cc[VMIN] = 1;
//         old.c_cc[VTIME] = 0;
//         if (tcsetattr(0, TCSANOW, &old) < 0)
//                 perror("tcsetattr ICANON");
//         if (read(0, &buf, 1) < 0)
//                 perror ("read()");
//         old.c_lflag |= ICANON;
//         old.c_lflag |= ECHO;
//         if (tcsetattr(0, TCSADRAIN, &old) < 0)
//                 perror ("tcsetattr ~ICANON");
//         return (buf);
// }


int getch()
{
    int ch;
    struct termios oldt;
    struct termios newt;

    // Store old settings, and copy to new settings
    tcgetattr(0, &oldt);
    newt = oldt;

    // Make required changes and apply the settings
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_iflag |= IGNBRK;
    newt.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF);
    newt.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 1;
    tcsetattr(fileno(stdin), TCSANOW, &newt);

    // Get the current character
    ch = getchar();

    // Reapply old settings
    tcsetattr(0, TCSANOW, &oldt);

    return ch;
}

// void sig_handler(int signum){
//    printf("Inside handler function\n");
//     signal(SIGALRM,sig_handler); // Register signal handler
//     useconds_t first = 200000;
//     useconds_t following = 200000;
// }

// char read_keyboard() {
//     char key = 0;
//     clock_t time_for_timeout(clock());
//     clock_t time_now(clock());
//     while ((float)(time_now - time_for_timeout)/CLOCKS_PER_SEC < 0.001){
//         std::cout << (float)(time_now - time_for_timeout)/CLOCKS_PER_SEC << std::endl;
//         signal(SIGALRM,sig_handler); // Register signal handler
//         key = getchar();
//         time_now = clock();
//     }
//     return key;
// }

int main(int argc, char **argv)
{
	ros::init(argc, argv, "keyboard_input_node");

	ros::NodeHandle nh;

	ros::Rate loop_rate(500);

	ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1);

	geometry_msgs::Twist twist;

	long count = 0;

    // signal(SIGALRM,sig_handler); // Register signal handler
    // useconds_t first = 200000;
    // useconds_t following = 200000;

	while (ros::ok())
	{
		twist.linear.x = 0.0;
		twist.linear.y = 0.0;
		twist.linear.z = 0.0;
		twist.angular.x = 0.0;
		twist.angular.y = 0.0;
		twist.angular.z = 0.0;

		int ch = 0; int ch1 = 0;
		// ch = read_keyboard();

        // ualarm(first, following);
        int msec = 0, trigger = 2000; /* 100ms */
        clock_t before = clock();

        do {
        /*
        * Do something to busy the CPU just here while you drink a coffee
        * Be sure this code will not take more than `trigger` ms
        */
        ch = getchar();
        std::cout << ch << std::endl;
        std::cout << msec << std::endl;
        // ch = ch1.back;
        clock_t difference = clock() - before;
        msec = difference * 1000 / CLOCKS_PER_SEC;
        } while ( msec < trigger );


		printf("%ld\n", count++);
		printf("ch = %d\n\n", ch);

		switch (ch)
		{
		case 'q':
			printf("already quit!\n");
			return 0;

		case 'w':
			twist.linear.x = 0.5;
			printf("move forward!\n");
			break;

		case 's':
			twist.linear.x = -0.5;
			printf("move backward!\n");
			break;

		case 'a':
			twist.linear.y = 0.5;
			printf("move left!\n");
			break;

		case 'd':
			twist.linear.y = -0.5;
			printf("move right!\n");
			break;

		case 'j':
			twist.angular.z = 1.0;
			printf("turn left!\n");
			break;

		case 'l':
			twist.angular.z = -1.0;
			printf("turn right!\n");
			break;

		default:
			printf("Stop!\n");
			break;
		}

        pub.publish(twist);

		ros::spinOnce();
		loop_rate.sleep();
	}

	return 0;
}
