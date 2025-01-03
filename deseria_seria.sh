
> ring R = 0, (w, x, y, z), (dp, c);
> newstruct("token", "list fieldnames, list data");
> link l="ssi: w serialize.ssi";
> ideal J = w2 - x * z, w* x - y * z, x2 - w * y, x* y - z2, y2 - w * z;
> string s = "{{1, 3}, {1, 2}, {1, 2}, {2, 4}, {3, 4}, {3, 4}}";
> token tok;
> tok.fieldnames=list("sparse_matrix", "tw_integers");
> tok.data= list(J, s);
> write(l,tok,"Ali");
> close(l);
> read(l);
data=<list>
fieldnames=<list>
>  def bla=_;
> bla;
data=<list>
fieldnames=<list>
> def r=bla.r_data;
> setring r;
> bla.data;
[1]:
   _[1]=w2-xz
   _[2]=wx-yz
   _[3]=x2-wy
   _[4]=xy-z2
   _[5]=y2-wz
[2]:
   {{1, 3}, {1, 2}, {1, 2}, {2, 4}, {3, 4}, {3, 4}}
> tok;
data=<list>
fieldnames=<list>
> tok.data;
   ? object from another ring
   ? error occurred in or before STDIN line 18: `tok.data;`
> setring R;
> tok;
data=<list>
fieldnames=<list>
> tok.data;
[1]:
   _[1]=w2-xz
   _[2]=wx-yz
   _[3]=x2-wy
   _[4]=xy-z2
   _[5]=y2-wz
[2]:
   {{1, 3}, {1, 2}, {1, 2}, {2, 4}, {3, 4}, {3, 4}}
> tok.data[2];
{{1, 3}, {1, 2}, {1, 2}, {2, 4}, {3, 4}, {3, 4}}
> typeof(tok.data[2]);
string



list LT = l;


  string path = "/home//atraore/gpi/try_gpi/feynman_ibp/example_dir/temp";  // Ensure the path ends with a "/"
  string filename = "Leadsyz.ssi";
  link l = "ssi:w " + path + filename;

  write(l, LT, "SecondSyzModule");
  close(l);
