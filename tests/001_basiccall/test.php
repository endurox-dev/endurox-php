<?

$buf = ndrx_tpalloc (NDRX_UBF, NULL, 1024);


if (0>ndrx_badd ($buf, "T_STRING_FLD", "HELLO WORLD", 0))
{
        print("Failed to add filed!\n");
}


if (0>ndrx_tpcall ("UNIXINFO", $buf, $buf, 0))
{
        print("call failed UNIXINFO: ".ndrx_tpstrerror(ndrx_get_tperrno())."\n");
}

$myfile = fopen("bufer.txt", "w");

/* for output see: /tmp/php_ndrx.out */
ndrx_bfprintf($buf);

$arr = ndrx_ubf2array ($buf, 0);

print_r($arr);

?>
