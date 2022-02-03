#include "os2021_thread_api.h"
#include <json-c/json.h>


int main(int argc,char** argv)
{
    setqueue();
    struct json_object *root=NULL,*t=NULL,*t2=NULL,*name=NULL,*entry=NULL,*priority=NULL,*cancel=NULL;
    root = json_object_from_file("./init_threads.json");
    t = json_object_object_get(root,"Thread");
    for(int i=0; i<json_object_array_length(t); i++)
    {
        t2 = json_object_array_get_idx(t,i);
        name = json_object_object_get(t2,"name");
        entry = json_object_object_get(t2,"entry function");
        priority = json_object_object_get(t2,"priority");
        cancel = json_object_object_get(t2,"cancel mode");
        //printf("%d\n",json_object_get_int(cancel));
        OS2021_ThreadCreate(
            json_object_get_string(name),
            json_object_get_string(entry),
            json_object_get_string(priority),
            json_object_get_int(cancel)
        );

    }
    OS2021_ThreadCreate("reclaimer","ResourceReclaim","L",1);
    StartSchedulingSimulation();
    json_object_put(root);
    json_object_put(t);
    json_object_put(t2);
    json_object_put(name);
    json_object_put(entry);
    json_object_put(priority);
    json_object_put(cancel);

    return 0;
}
