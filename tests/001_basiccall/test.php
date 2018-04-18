<?

$buf = ndrxph_tpalloc (NDRXPH_UBF, NULL, 1024);


if (0>ndrxph_badd ($buf, "T_STRING_FLD", "HELLO WORLD", 0))
{
        print("Failed to add filed!\n");
}


if (0>ndrxph_tpcall ("UNIXINFO", $buf, $buf, 0))
{
        print("call failed UNIXINFO: ".ndrxph_tpstrerror(ndrxph_get_tperrno())."\n");
}

/* for output see: /tmp/php_ndrx.out */
ndrxph_bfprintf($buf);

$arr = ndrxph_ubf2array ($buf, 0);

print_r($arr);

?>
