#ifndef __DUMP_H__
#define __DUMP_H__

void save_state(char *fname,dict* kv_dict,dict* set_dict);
void load_state(char *fname,dict* kv_dict,dict* set_dict);

#endif
