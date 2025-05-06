#include "singular_functions.hpp"
#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <chrono>
// implementation
const std::string STRUCT_NAME = "token";
const std::string STRUCT_DESC = "list fieldnames, list data";

// implementation
void call_singular(std::string const& command) {
    int err = iiAllStart(NULL, const_cast<char*>(command.c_str()), BT_proc, 0);
    if (err) {
        errorreported = 0;
        throw std::runtime_error("Singular returned an error ...");
    }
}

// implementation
void call_singular_and_discard(std::string const& command) {
    SPrintStart();
    call_singular(command);
    char* result_ptr = SPrintEnd();
    omFree(result_ptr);
}

// implementation // To be modified filename_generaor
std::string filename_generator()
{
  char hst[65];
  uint64_t current_time;
  gethostname(hst,64);
  hst[64] = '\0';
  std::string filename (hst);
  filename = filename + '_' +  std::to_string(getpid());
  current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  filename = filename + '_' + std::to_string(current_time);
  return  filename;
}

// implementation
si_link ssi_open_for_write(std::string const& ssi_filename) {
    si_link l = static_cast<si_link>(omAlloc0Bin(sip_link_bin));
    l->name = omStrDup(ssi_filename.c_str());
    l->mode = omStrDup("w");
    l->ref = 1;
    si_link_extension ns = static_cast<si_link_extension>(omAlloc0Bin(s_si_link_extension_bin));
    ns = slInitSsiExtension(ns);
    l->m = ns;

    BOOLEAN res = l->m->Open(l, SI_LINK_WRITE, NULL);
    // this already writes the parameter line "98 ..."

    if (res) {
        throw std::runtime_error("could not open ssi file " + ssi_filename + " for write");
    }

    return l;
}

// implementation
void ssi_close_and_remove(si_link l) {
    BOOLEAN res = l->m->Close(l); // this frees l->d
    if (res) {
        throw std::runtime_error("closing ssi link failed");
    }
    omFree(static_cast<ADDRESS>(l->name));
    omFree(static_cast<ADDRESS>(l->mode));
    omFreeBin(l->m, s_si_link_extension_bin);
    omFreeBin(l, sip_link_bin);
}

// implementation
void ssi_write_newstruct(si_link l, std::string const& struct_name, lists lst) {
    std::cout << "Writing newstruct '" << struct_name << "' with list at " << (void*)lst << std::endl;
    if (!l || !lst) {
        throw std::runtime_error("Invalid link or list in ssi_write_newstruct");
    }
    int type;
    if (!blackboxIsCmd(struct_name.c_str(), type)) {
        throw std::runtime_error("Failed to get blackbox type for " + struct_name);
    }
    std::cout << "Blackbox type for " << struct_name << ": " << type << std::endl;
    ssi_write_newstruct(l, type, lst);
}

void ssi_write_newstruct(si_link l, int type, lists lst) {
    std::cout << "Writing newstruct with type " << type << std::endl;
    ssiInfo* d = static_cast<ssiInfo*>(l->data);
    if (!d || !d->f_write) {
        throw std::runtime_error("Invalid SSI data or file pointer");
    }
    blackbox* b = getBlackboxStuff(type);
    if (!b) {
        throw std::runtime_error("Failed to get blackbox for type " + std::to_string(type));
    }
    fputs("20 ", d->f_write); // Write newstruct token
    newstruct_serialize(b, lst, l); // Use Singular's serialization
    std::cout << "Completed newstruct_serialize" << std::endl;
}
std::string serialize(lists lst, std::string const& base_filename) {
    std::cout << "Starting serialization for list at " << (void*)lst << std::endl;
    if (!lst) {
        throw std::runtime_error("Attempt to serialize null list");
    }
    std::cout << "List size: " << lst->nr + 1 << std::endl;

    std::string out_filename = filename_generator();
    out_filename = base_filename + out_filename;
    si_link l = ssi_open_for_write(out_filename);
    std::cout << "Opened SSI link for writing: " << out_filename << std::endl;

    // Write as a simple list
    ssiInfo* d = static_cast<ssiInfo*>(l->data);
    fprintf(d->f_write, "20 %d ", LIST_CMD); // Write token and type
    fprintf(d->f_write, "%d ", 4); // Write string length for "list"
    fputs("list", d->f_write); // Write the string "list"
    fprintf(d->f_write, " %d ", lst->nr + 1); // Write list length
    char* str = lString(lst);
    std::cout << "List string format: " << str << std::endl;
    fputs(str, d->f_write);
    fputs("\n", d->f_write);
    omFree(str);
    std::cout << "Wrote list to SSI" << std::endl;

    ssi_close_and_remove(l);
    std::cout << "Closed and removed SSI link" << std::endl;

    return out_filename;
}
lists ssi_read_newstruct(si_link l, std::string const& struct_name) {
    ssiInfo* d = static_cast<ssiInfo*>(l->data);
    int t = s_readint(d->f_read);
    if (t != 20) {
        throw std::runtime_error("wrong token, expected 20 got " + std::to_string(t));
    }
    int type = s_readint(d->f_read); // Read type ID
    std::string name = ssiReadString(d);

    if (name != "list") {
        throw std::runtime_error("wrong name, expected list got " + name);
    }

    lists li = (lists)omAlloc0(sizeof(slists));
    if (type == LIST_CMD) {
        // Read the list length
        int length = s_readint(d->f_read);
        std::cout << "Reading list of length: " << length << std::endl;

        // Skip the space
        s_getc(d->f_read);

        // Allocate buffer for the list string
        char* buffer = static_cast<char*>(omAlloc0(length * 20 + 1)); // Increased buffer size for safety
        s_readbytes(buffer, length * 20, d->f_read);
        buffer[length * 20] = '\0';
        std::cout << "Read buffer: " << buffer << std::endl;

        // Parse the string manually (format: "1,2,3")
        li->Init(length);
        int val = 0, idx = 0;
        bool in_number = false;
        for (int i = 0; buffer[i] != '\0' && idx < length; i++) {
            if (buffer[i] >= '0' && buffer[i] <= '9') {
                val = val * 10 + (buffer[i] - '0');
                in_number = true;
            } else if ((buffer[i] == ',' || buffer[i] == '\0') && in_number) {
                std::cout << "Parsed value: " << val << std::endl;
                li->m[idx].rtyp = INT_CMD;
                li->m[idx].data = (void*)(long)val;
                idx++;
                val = 0;
                in_number = false;
            }
        }
        // Handle the last number if we're still in a number
        if (in_number && idx < length) {
            std::cout << "Parsed value: " << val << std::endl;
            li->m[idx].rtyp = INT_CMD;
            li->m[idx].data = (void*)(long)val;
        }
        omFree(buffer);
    } else {
        throw std::runtime_error("Unsupported type " + std::to_string(type));
    }
    return li;
}

// implementation
bool register_struct(std::string const& name, std::string const& desc) {
    int tok;
    int cmd_result = blackboxIsCmd(name.c_str(), tok);
    if (cmd_result == ROOT_DECL) {
        return true; // already exists
    }

    newstruct_desc d = newstructFromString(desc.c_str());
    if (d == NULL) {
        return false;
    }
    newstruct_setup(name.c_str(), d); // is void function
    return true;
}

// implementation
si_link ssi_open_for_read(std::string const& ssi_filename) {
    si_link l = static_cast<si_link>(omAlloc0Bin(sip_link_bin));
    l->name = omStrDup(ssi_filename.c_str());
    l->mode = omStrDup("r");
    l->ref = 1;
    si_link_extension ns = static_cast<si_link_extension>(omAlloc0Bin(s_si_link_extension_bin));
    ns = slInitSsiExtension(ns);
    l->m = ns; // originally, Singular has a global list of "extensions"
               // we use a private copy for now

    BOOLEAN res = l->m->Open(l, SI_LINK_READ, NULL);
    if (res) {
        throw std::runtime_error("could not open ssi file " + ssi_filename);
    }

    ssiInfo* d = static_cast<ssiInfo*>(l->data);
    int t = s_readint(d->f_read); // SSI version info in first line
    if (t != 98) {
        std::string error_msg = "wrong token, got " + std::to_string(t);
        throw std::runtime_error(error_msg);
    }
    int n98_v, n98_m;
    BITSET n98_o1, n98_o2;
    n98_v = s_readint(d->f_read);
    n98_m = s_readint(d->f_read);
    n98_o1 = s_readint(d->f_read);
    n98_o2 = s_readint(d->f_read);

//  std::cout << "versions of ssi:" << SSI_VERSION<<std::endl;
   
    if ((n98_v != SSI_VERSION) || (n98_m != MAX_TOK)) {
        std::string error_msg = "incompatible versions of ssi: expected " +
                                std::to_string(SSI_VERSION) + '/' + std::to_string(MAX_TOK) +
                                " got " + std::to_string(n98_v) + '/' + std::to_string(n98_m);
        throw std::runtime_error(error_msg);
    }
    si_opt_1 = n98_o1; // do I want to set these global options?
    si_opt_2 = n98_o2;

    return l;
}

// implementation
std::string ssiReadString(const ssiInfo* d) {
    // Read the length of the string from the file
    int length = s_readint(d->f_read);
    
    // Validate the length
    if (length < 0) {
        throw std::runtime_error("Invalid string length");
    }
    
    // Allocate memory for the string including the null terminator
    char* buffer = static_cast<char*>(omAlloc0(length + 1));

    // Handle memory allocation failure
    if (buffer == nullptr) {
        throw std::bad_alloc();
    }

    // Skip the first byte (likely a space or delimiter) if necessary
    int throwaway = s_getc(d->f_read); // Skip the ' ' or delimiter
    (void)throwaway;  // Silence the unused variable warning

    // Read the actual string bytes
    s_readbytes(buffer, length, d->f_read);

    // Ensure null-termination
    buffer[length] = '\0';

    // Convert the C-string buffer to std::string and return it
    std::string result(buffer);

    // Free allocated memory
    omFree(buffer);

    return result;
}


// implementation

std::pair<int, lists> deserialize(std::string const& filename, std::string const& ids) {
    // Register the data structure
    if (!(register_struct(STRUCT_NAME, STRUCT_DESC))) {
        throw std::runtime_error(ids + ": singular_parallel_all_compute: could not register structs");
    }

    int type;
    // Get the type of the structure
    blackboxIsCmd(STRUCT_NAME.c_str(), type);
   // std::cout << "Deserializing file: " << filename << std::endl;
    //std::cout << "Retrieved type: " << type << std::endl;

    // Open the file for reading
    si_link l = ssi_open_for_read(filename);
    if (!l) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Read the structure from the file
    lists lst = ssi_read_newstruct(l, STRUCT_NAME);
    if (!lst) {
        ssi_close_and_remove(l);
        throw std::runtime_error("Failed to read structure from file: " + filename);
    }

    // Close and remove the file
    ssi_close_and_remove(l);

    // Debug output for the deserialized list
    //std::cout << "Deserialized lists from file: " << filename << std::endl;

    // Return the type and the lists structure
    return { type, lst };
}

void load_singular_library(std::string const& library_name) {
    char* s = omStrDup(library_name.c_str());
    std::cout << "Attempting to load library: " << s << std::endl;
    BOOLEAN res = iiGetLibStatus(s);
    if (res == TRUE) { // already loaded
        std::cout << "Library " << s << " already loaded" << std::endl;
        omFree(s);
        return;
    }
    
    // Try to load the library with the full path
    res = iiLibCmd(s, TRUE, FALSE, FALSE); // also frees s
    if (res) {
        // If loading with full path failed, try loading just the library name
        std::string lib_name = library_name.substr(library_name.find_last_of("/") + 1);
        s = omStrDup(lib_name.c_str());
        res = iiLibCmd(s, TRUE, FALSE, FALSE);
        if (res) {
            std::string error_msg = "could not load library " + library_name + " or " + lib_name;
            std::cerr << error_msg << std::endl;
            throw std::runtime_error(error_msg);
        }
    }
    std::cout << "Successfully loaded library: " << library_name << std::endl;
}

// implementation
idhdl symbol(std::string const& lib, std::string const& fun) {
    load_singular_library(lib);
    idhdl h = ggetid(fun.c_str());
    if (h == NULL) {
        throw std::runtime_error("procedure " + fun + " not available in " + lib);
    }
    return h;
}

// implementation
ScopedLeftv::ScopedLeftv(int c, void* data)
    : _(static_cast<leftv>(omAlloc0Bin(sleftv_bin))) {
    _->rtyp = c;
    _->data = data;
}
// implementation
ScopedLeftv::ScopedLeftv(ScopedLeftv& parent, int c, void* data)
    : ScopedLeftv(c, data) {
    chained = true;
    if (parent._->next == NULL)
        parent._->next = _;
    else
        parent._->next->next = _;
}
// implementation
ScopedLeftv::~ScopedLeftv() {
    if (!chained) {
        _->CleanUp();
        omFreeBin(_, sleftv_bin);
    }
}

// implementation
leftv ScopedLeftv::leftV() const {
    return _;
}

// implementation
void init_singular(std::string const& library_path) {
    if (currPack == NULL) // use this to check if this instance has already been
                         // initializied
    {
        mp_set_memory_functions(omMallocFunc, omReallocSizeFunc, omFreeSizeFunc);
        siInit(const_cast<char*>(library_path.c_str()));
        currentVoice = feInitStdin(NULL);
        errorreported = 0;
        myynest = 1;
    }
}

// implementation
template<typename R>
std::pair<int, R> proc(idhdl h, ScopedLeftv const& arg) {
    BOOLEAN const res(iiMake_proc(h, NULL, arg.leftV()));

    if (res) {
        throw std::runtime_error("call to procedure " + std::string(h->id) +
                                 " failed");
    }

    int const i = iiRETURNEXPR.Typ();
    std::cout << "Return type: " << i << std::endl;
    
    lists src = static_cast<lists>(iiRETURNEXPR.Data());
    std::cout << "Source list size: " << src->nr + 1 << std::endl;
    
    // Use Singular's built-in list copying function
    lists dst = lCopy(src);
    std::cout << "List copy completed" << std::endl;
    
    R const r = static_cast<R>(dst);
    std::cout << "Cast to return type completed" << std::endl;

    iiRETURNEXPR.data = NULL;
    iiRETURNEXPR.CleanUp();
    std::cout << "Return expression cleaned up" << std::endl;

    return std::make_pair(i, r);
}

// implementation
std::pair<int, lists> call_user_proc(std::string const& function_name,
                                     std::string const& needed_library, int in_type, lists in_lst) {
    ScopedLeftv arg(in_type, lCopy(in_lst));
    return proc<lists>(symbol(needed_library, function_name), arg);
}

// implementation
std::pair<int, lists> call_user_proc(std::string const& function_name,
                                     std::string const& needed_library, ScopedLeftv& u_arg) {
    return proc<lists>(symbol(needed_library, function_name), u_arg);
}

// implementation
std::string worker() {
    char hstn[65];
    gethostname(hstn, 64);
    hstn[64] = '\0';
    std::string ids(hstn);
    ids = ids + " " + std::to_string(getpid());
    return ids;
}
//implementation
// std::list wrap(std::list<std::string> const& cpp_list) {
//     std::list wrapped_list;

//     for (auto const& str : cpp_list) {
//         wrapped_list.push_back(str);
//     }

//     return wrapped_list;
// }