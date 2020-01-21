/**
 * Test corresponding to the scheduling capacities
 * for docker Module 
 * */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <time.h> 
#include "shared.h"
#include "wazuh_modules/wmodules.h"
#include "wazuh_modules/wm_docker.h"
#include "wmodules_scheduling_helpers.h"

#define TEST_MAX_DATES 5

static wmodule *docker_module;
static OS_XML *lxml;

static unsigned test_docker_date_counter = 0;
static struct tm test_docker_date_storage[TEST_MAX_DATES];

int __wrap_wpclose(wfd_t * wfd) {
    if (wfd->file) {
        fclose(wfd->file);
    }
    free(wfd);
    time_t current_time = time(NULL);
    struct tm *date = localtime(&current_time);
    test_docker_date_storage[test_docker_date_counter++] = *date;
    if(test_docker_date_counter >= TEST_MAX_DATES){
        const wm_docker_t *ptr = (wm_docker_t *) docker_module->data;
        check_function_ptr( &ptr->scan_config, &test_docker_date_storage[0], TEST_MAX_DATES);
        // Break infinite loop
        disable_forever_loop();
    }
    return 0;
}

wfd_t * __wrap_wpopenl(const char * path, int flags, ...) {
    wfd_t * wfd;
    os_calloc(1, sizeof(wfd_t), wfd);
    return wfd;
}

char *__wrap_fgets (char *__restrict __s, int __n, FILE *__restrict __stream) {
    return 0;
}

/******* Helpers **********/

static void set_up_test(void (*ptr)(const sched_scan_config *scan_config, struct tm *date_array, unsigned int MAX_DATES)) {
    enable_forever_loop();
    wm_max_eps = 1;
    test_docker_date_counter = 0;
    check_function_ptr = ptr;
}

static void wmodule_cleanup(wmodule *module){
    free(module->data);
    free(module->tag);
    free(module);
}

/***  SETUPS/TEARDOWNS  ******/
static int setup_module() {
    docker_module = calloc(1, sizeof(wmodule));
    const char *string = 
        "<interval>10m</interval>\n"
        "<attempts>10</attempts>\n"
        "<run_on_start>no</run_on_start>\n"
        "<disabled>no</disabled>\n";
    lxml = malloc(sizeof(OS_XML));
    XML_NODE nodes = string_to_xml_node(string, lxml);
    assert_int_equal(wm_docker_read(nodes, docker_module), 0);
    OS_ClearNode(nodes);
    return 0;
}

static int teardown_module(){
    wmodule_cleanup(docker_module);
    OS_ClearXML(lxml);
    return 0;
}

/****************************************************************/

/** Tests **/
void test_interval_execution() {
    set_up_test(check_time_interval);
    wm_docker_t* module_data = (wm_docker_t *)docker_module->data;
    module_data->scan_config.last_scan_time = 0;
    module_data->scan_config.scan_day = 0;
    module_data->scan_config.scan_wday = -1;
    module_data->scan_config.interval = 60 * 25; // 25min
    module_data->scan_config.month_interval = false;
    docker_module->context->start(module_data);
}

void test_day_of_month() {
    set_up_test(check_day_of_month);
    wm_docker_t* module_data = (wm_docker_t *)docker_module->data;
    module_data->scan_config.last_scan_time = 0;
    module_data->scan_config.scan_day = 27;
    module_data->scan_config.scan_wday = -1;
    module_data->scan_config.scan_time = strdup("00:00");
    module_data->scan_config.interval = 1; // 1 month
    module_data->scan_config.month_interval = true;
    docker_module->context->start(module_data);
    free(module_data->scan_config.scan_time);
}

void test_day_of_week() {
    set_up_test(check_day_of_week);
    wm_docker_t* module_data = (wm_docker_t *)docker_module->data;
    module_data->scan_config.last_scan_time = 0;
    module_data->scan_config.scan_day = 0;
    module_data->scan_config.scan_wday = 0;
    module_data->scan_config.scan_time = strdup("00:00");
    module_data->scan_config.interval = 604800;  // 1 week
    module_data->scan_config.month_interval = false;
    docker_module->context->start(module_data);
    free(module_data->scan_config.scan_time);
}

void test_time_of_day() {
    set_up_test(check_time_of_day);
    wm_docker_t* module_data = (wm_docker_t *)docker_module->data;
    module_data->scan_config.last_scan_time = 0;
    module_data->scan_config.scan_day = 0;
    module_data->scan_config.scan_wday = -1;
    module_data->scan_config.scan_time = strdup("00:00");
    module_data->scan_config.interval = WM_DEF_INTERVAL;  // 1 day
    module_data->scan_config.month_interval = false;
    docker_module->context->start(module_data);
    free(module_data->scan_config.scan_time);
}

void test_fake_tag() {
    set_up_test(check_time_of_day);
    const char *string =
        "<time>19:55</time>\n"
        "<interval>10m</interval>\n"
        "<attempts>10</attempts>\n"
        "<run_on_start>no</run_on_start>\n"
        "<disabled>no</disabled>\n"
        "<extra-tag>extra</extra-tag>\n";
    wmodule *module = calloc(1, sizeof(wmodule));
    OS_XML xml;
    XML_NODE nodes = string_to_xml_node(string, &xml);
    assert_int_equal(wm_docker_read(nodes, module),-1);
    OS_ClearNode(nodes);
    OS_ClearXML(&xml);
    wm_docker_t* module_data = (wm_docker_t *)module->data;
    free(module_data->scan_config.scan_time);
    wmodule_cleanup(module);
}

void test_read_scheduling_monthday_configuration() {
    const char *string = 
        "<time>19:55</time>\n"
        "<day>10</day>\n"
        "<attempts>10</attempts>\n"
        "<run_on_start>no</run_on_start>\n"
        "<disabled>no</disabled>\n"
    ;
    wmodule *module = calloc(1, sizeof(wmodule));;
    OS_XML xml;
    XML_NODE nodes = string_to_xml_node(string, &xml);
    assert_int_equal(wm_docker_read(nodes, module),0);
    wm_docker_t *module_data = (wm_docker_t*)module->data;
    assert_int_equal(module_data->scan_config.scan_day, 10);
    assert_int_equal(module_data->scan_config.interval, 1);
    assert_int_equal(module_data->scan_config.month_interval, true);
    assert_int_equal(module_data->scan_config.scan_wday, -1);
    assert_string_equal(module_data->scan_config.scan_time, "19:55");
    OS_ClearNode(nodes);
    OS_ClearXML(&xml);
    free(module_data->scan_config.scan_time);
    wmodule_cleanup(module);
}

void test_read_scheduling_weekday_configuration() {
    const char *string = 
        "<time>18:55</time>\n"
        "<wday>Thursday</wday>\n"
        "<attempts>10</attempts>\n"
        "<run_on_start>no</run_on_start>\n"
        "<disabled>no</disabled>\n"
    ;
    wmodule *module = calloc(1, sizeof(wmodule));
    OS_XML xml;
    XML_NODE nodes = string_to_xml_node(string, &xml);
    assert_int_equal(wm_docker_read(nodes, module),0);
    wm_docker_t *module_data = (wm_docker_t*)module->data;
    assert_int_equal(module_data->scan_config.scan_day, 0);
    assert_int_equal(module_data->scan_config.interval, 604800);
    assert_int_equal(module_data->scan_config.month_interval, false);
    assert_int_equal(module_data->scan_config.scan_wday, 4);
    assert_string_equal(module_data->scan_config.scan_time, "18:55");
    OS_ClearNode(nodes);
    OS_ClearXML(&xml);
    free(module_data->scan_config.scan_time);
    wmodule_cleanup(module);
}

void test_read_scheduling_daytime_configuration() {
    const char *string = 
        "<time>17:20</time>\n"
        "<attempts>10</attempts>\n"
        "<run_on_start>no</run_on_start>\n"
        "<disabled>no</disabled>\n"
    ;
    wmodule *module = calloc(1, sizeof(wmodule));;
    OS_XML xml;
    XML_NODE nodes = string_to_xml_node(string, &xml);
    assert_int_equal(wm_docker_read(nodes, module),0);
    wm_docker_t *module_data = (wm_docker_t*)module->data;
    assert_int_equal(module_data->scan_config.scan_day, 0);
    assert_int_equal(module_data->scan_config.interval, WM_DEF_INTERVAL);
    assert_int_equal(module_data->scan_config.month_interval, false);
    assert_int_equal(module_data->scan_config.scan_wday, -1);
    assert_string_equal(module_data->scan_config.scan_time, "17:20");
    OS_ClearNode(nodes);
    OS_ClearXML(&xml);
    free(module_data->scan_config.scan_time);
    wmodule_cleanup(module);
}

void test_read_scheduling_interval_configuration() {
    const char *string = 
        "<interval>1d</interval>\n"
        "<attempts>10</attempts>\n"
        "<run_on_start>no</run_on_start>\n"
        "<disabled>no</disabled>\n"
    ;
    wmodule *module = calloc(1, sizeof(wmodule));;
    OS_XML xml;
    XML_NODE nodes = string_to_xml_node(string, &xml);
    assert_int_equal(wm_docker_read(nodes, module),0);
    wm_docker_t *module_data = (wm_docker_t*)module->data;
    assert_int_equal(module_data->scan_config.scan_day, 0);
    assert_int_equal(module_data->scan_config.interval, WM_DEF_INTERVAL); // 1 day
    assert_int_equal(module_data->scan_config.month_interval, false);
    assert_int_equal(module_data->scan_config.scan_wday, -1);
    OS_ClearNode(nodes);
    OS_ClearXML(&xml);
    wmodule_cleanup(module);
}

int main(void) {
    const struct CMUnitTest tests_with_startup[] = {
        cmocka_unit_test(test_interval_execution),
        cmocka_unit_test(test_day_of_month),
        cmocka_unit_test(test_day_of_week),
        cmocka_unit_test(test_time_of_day)
    };
    const struct CMUnitTest tests_without_startup[] = {
        cmocka_unit_test(test_fake_tag),
        cmocka_unit_test(test_read_scheduling_monthday_configuration),
        cmocka_unit_test(test_read_scheduling_weekday_configuration),
        cmocka_unit_test(test_read_scheduling_daytime_configuration),
        cmocka_unit_test(test_read_scheduling_interval_configuration)
    };
    int result;
    result = cmocka_run_group_tests(tests_with_startup, setup_module, teardown_module);
    result &= cmocka_run_group_tests(tests_without_startup, NULL, NULL);
    return result;
}
